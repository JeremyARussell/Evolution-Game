#ifndef CELLS_AND_BODIES_H
#define CELLS_AND_BODIES_H
#pragma once

#include <vector>
#include <cmath>

// ---------------------------------------------------------------------------
// Collision filter categories
// ---------------------------------------------------------------------------
enum _CellCategory {
    CB_WALL     = 1,
    CB_BONE     = 2,
    CB_LIGAMENT = 4,
    CB_MUSCLE   = 8,
};

// ---------------------------------------------------------------------------
// Cell type and mode enums / data structs
// ---------------------------------------------------------------------------
enum CellType  { CT_BONE = 0, CT_LIGAMENT = 1, CT_MUSCLE = 2 };
enum BuildMode { BM_BUILDING, BM_SIMULATING };

// One placed cell in the creature
struct CreatureCell {
    b2Body*  body;
    CellType type;
    int      gx, gy;  // integer grid position (world units)
};

// A distance-joint connection between two cells (ligament, muscle, bone-to-non-bone)
struct CellJoint {
    b2DistanceJoint* joint;
    int     idxA, idxB;       // indices into m_cells
    bool    isMuscle;         // true if either endpoint is a muscle cell
    float32 relaxedLen;       // natural rest length
    float32 contractedLen;    // shortened rest length when muscles fire
};

// A weld-joint connection between two bone cells (locks position + angle)
struct CellWeld {
    b2WeldJoint* joint;
    int idxA, idxB;
};

// ---------------------------------------------------------------------------
// Local AABB query callback — used for mouse-grab in simulate mode
// ---------------------------------------------------------------------------
class CellQueryCallback : public b2QueryCallback {
public:
    CellQueryCallback(const b2Vec2& point) : m_point(point), m_fixture(NULL) {}

    bool ReportFixture(b2Fixture* fixture) {
        if (fixture->GetBody()->GetType() == b2_dynamicBody &&
            fixture->TestPoint(m_point)) {
            m_fixture = fixture;
            return false;
        }
        return true;
    }

    b2Vec2    m_point;
    b2Fixture* m_fixture;
};

// ---------------------------------------------------------------------------
// CellsAndBodies — grid-based soft-body creature builder
//
// BUILD MODE (default)
// --------------------
//   Left-click      — place cell of the selected type at the nearest grid position
//   [1] [2] [3]     — select Bone / Ligament / Muscle
//   [X]             — toggle erase mode (left-click removes cells)
//   [G]             — "Go": release the creature into simulation
//   [C]             — clear and return to build mode
//
// SIMULATE MODE
// -------------
//   [Space]         — fire / relax all muscles
//   [C]             — clear and return to build mode
//
// Cell types
// ----------
//   Bone      (grey)   — dense, hard.  Stiff distance joints to any neighbour.
//   Ligament  (yellow) — light, elastic.  Soft spring joints that flex under load.
//   Muscle    (red)    — contracts on [Space], shortening any joint it touches.
//
// Physics model
// -------------
//   Cells are b2_staticBody while building (so they sit in place).
//   On [G], all cells switch to b2_dynamicBody and fall under gravity.
//   Bone-bone connections use b2WeldJoint (locks position + angle) → rigid.
//   All other connections are b2DistanceJoint with spring (frequencyHz > 0).
//   Muscle contraction: SetLength() toggles the rest length of muscle joints.
//   collideConnected = false suppresses jitter between directly linked cells;
//   non-adjacent cells of the same creature still collide normally.
// ---------------------------------------------------------------------------
class CellsAndBodies : public World {
public:

    // ---- GUI stubs (required by World interface) ----
    void nextWheeler()    {}
    void previusWheeler() {}
    void destroyCreature(){}
    void saveWorld()      {}
    void loadWorld()      {}
    void exportCreature() {}
    void importCreature() {}

    // ---- Tunable constants ----
    static const float32 WORLD_HALF;    // half-extent of the bounded area
    static const float32 CELL_RADIUS;   // physics + visual radius of each cell
    static const float32 CONTRACT_RATIO;// fraction by which muscles shorten (0..1)

    BuildMode m_mode;
    CellType  m_selectedType;
    bool      m_eraseMode;
    bool      m_musclesContracted;
    bool      m_firstStep;

    std::vector<CreatureCell> m_cells;
    std::vector<CellJoint>    m_joints;   // distance joints (elastic / muscle)
    std::vector<CellWeld>     m_welds;    // weld joints (bone–bone only)

    b2Body* m_floorBody;  // static floor + wall geometry (separate from base m_groundBody)

    // ------------------------------------------------------------------ //
    CellsAndBodies()
        : m_mode(BM_BUILDING)
        , m_selectedType(CT_BONE)
        , m_eraseMode(false)
        , m_musclesContracted(false)
        , m_firstStep(true)
        , m_floorBody(NULL)
    {
        activePower = GRAB;

        m_world->SetGravity(b2Vec2(0.0f, -10.0f));

        top    =  (int)WORLD_HALF;
        left   = -(int)WORLD_HALF;
        bottom = -(int)WORLD_HALF;
        right  =  (int)WORLD_HALF;

        buildFloorAndWalls();
    }

    ~CellsAndBodies() {
        // Vectors are cleaned up automatically; Box2D bodies/joints are
        // destroyed by the base class when it deletes m_world.
        m_cells.clear();
        m_joints.clear();
    }

    // ------------------------------------------------------------------ //
    //  World construction
    // ------------------------------------------------------------------ //

    void buildFloorAndWalls() {
        b2BodyDef bd;   // default = b2_staticBody
        m_floorBody = m_world->CreateBody(&bd);

        b2FixtureDef fd;
        fd.friction            = 0.7f;
        fd.filter.categoryBits = CB_WALL;
        fd.filter.maskBits     = CB_BONE | CB_LIGAMENT | CB_MUSCLE;

        // Floor: top surface at y = -5
        b2PolygonShape floor;
        floor.SetAsBox(WORLD_HALF, 1.0f, b2Vec2(0.0f, -6.0f), 0.0f);
        fd.shape = &floor;
        m_floorBody->CreateFixture(&fd);

        // Left wall
        b2PolygonShape leftWall;
        leftWall.SetAsBox(1.0f, WORLD_HALF + 1.0f,
                          b2Vec2(-WORLD_HALF - 1.0f, 0.0f), 0.0f);
        fd.shape = &leftWall;
        m_floorBody->CreateFixture(&fd);

        // Right wall
        b2PolygonShape rightWall;
        rightWall.SetAsBox(1.0f, WORLD_HALF + 1.0f,
                           b2Vec2(WORLD_HALF + 1.0f, 0.0f), 0.0f);
        fd.shape = &rightWall;
        m_floorBody->CreateFixture(&fd);
    }

    // ------------------------------------------------------------------ //
    //  Grid helpers
    // ------------------------------------------------------------------ //

    // Nearest integer, works correctly for negative values
    static int gRound(float32 v) {
        return (int)floorf(v + 0.5f);
    }

    // Returns index into m_cells, or -1 if the grid square is empty
    int findCell(int gx, int gy) const {
        for (int i = 0; i < (int)m_cells.size(); i++)
            if (m_cells[i].gx == gx && m_cells[i].gy == gy) return i;
        return -1;
    }

    // ------------------------------------------------------------------ //
    //  Build mode: place / remove cells
    // ------------------------------------------------------------------ //

    void placeCell(int gx, int gy) {
        if (m_mode != BM_BUILDING) return;

        if (m_eraseMode) {
            removeCell(gx, gy);
            return;
        }

        if (gy < -4) return;           // below the floor
        if (findCell(gx, gy) >= 0) return; // already occupied

        b2BodyDef bd;
        bd.type = b2_staticBody;       // static while building
        bd.position.Set((float32)gx, (float32)gy);
        b2Body* body = m_world->CreateBody(&bd);

        b2CircleShape circle;
        circle.m_radius = CELL_RADIUS;

        b2FixtureDef fd;
        fd.shape = &circle;

        switch (m_selectedType) {
        case CT_BONE:
            fd.density     = 2.0f;
            fd.friction    = 0.5f;
            fd.restitution = 0.1f;
            fd.filter.categoryBits = CB_BONE;
            fd.filter.maskBits     = CB_WALL | CB_BONE | CB_LIGAMENT | CB_MUSCLE;
            break;
        case CT_LIGAMENT:
            fd.density     = 0.6f;
            fd.friction    = 0.2f;
            fd.restitution = 0.3f;
            fd.filter.categoryBits = CB_LIGAMENT;
            fd.filter.maskBits     = CB_WALL | CB_BONE | CB_LIGAMENT | CB_MUSCLE;
            break;
        case CT_MUSCLE:
            fd.density     = 1.2f;
            fd.friction    = 0.4f;
            fd.restitution = 0.1f;
            fd.filter.categoryBits = CB_MUSCLE;
            fd.filter.maskBits     = CB_WALL | CB_BONE | CB_LIGAMENT | CB_MUSCLE;
            break;
        }

        body->CreateFixture(&fd);

        CreatureCell cell;
        cell.body = body;
        cell.type = m_selectedType;
        cell.gx   = gx;
        cell.gy   = gy;
        int newIdx = (int)m_cells.size();
        m_cells.push_back(cell);

        // Auto-connect to the 4 orthogonally adjacent cells
        const int dx[] = { 1, -1,  0, 0 };
        const int dy[] = { 0,  0, -1, 1 };
        for (int d = 0; d < 4; d++) {
            int adj = findCell(gx + dx[d], gy + dy[d]);
            if (adj >= 0)
                connectCells(newIdx, adj);
        }
    }

    void removeCell(int gx, int gy) {
        int idx = findCell(gx, gy);
        if (idx < 0) return;

        // Destroy all distance joints that reference this cell
        for (int i = (int)m_joints.size() - 1; i >= 0; i--) {
            if (m_joints[i].idxA == idx || m_joints[i].idxB == idx) {
                m_world->DestroyJoint(m_joints[i].joint);
                m_joints.erase(m_joints.begin() + i);
            }
        }

        // Destroy all weld joints that reference this cell
        for (int i = (int)m_welds.size() - 1; i >= 0; i--) {
            if (m_welds[i].idxA == idx || m_welds[i].idxB == idx) {
                m_world->DestroyJoint(m_welds[i].joint);
                m_welds.erase(m_welds.begin() + i);
            }
        }

        // Fix up indices in remaining joints and welds (everything > idx shifts down)
        for (auto& j : m_joints) {
            if (j.idxA > idx) j.idxA--;
            if (j.idxB > idx) j.idxB--;
        }
        for (auto& w : m_welds) {
            if (w.idxA > idx) w.idxA--;
            if (w.idxB > idx) w.idxB--;
        }

        m_world->DestroyBody(m_cells[idx].body);
        m_cells.erase(m_cells.begin() + idx);
    }

    void clearCreature() {
        // Joints and welds must be destroyed before bodies
        for (auto& cj : m_joints)
            m_world->DestroyJoint(cj.joint);
        m_joints.clear();

        for (auto& cw : m_welds)
            m_world->DestroyJoint(cw.joint);
        m_welds.clear();

        for (auto& c : m_cells)
            m_world->DestroyBody(c.body);
        m_cells.clear();

        m_mode             = BM_BUILDING;
        m_musclesContracted = false;
        m_eraseMode        = false;
    }

    // ------------------------------------------------------------------ //
    //  Create a joint between two cells
    //  Bone-bone  → b2WeldJoint  (locks position + angle, truly rigid)
    //  Everything else → b2DistanceJoint with spring
    // ------------------------------------------------------------------ //

    void connectCells(int idxA, int idxB) {
        b2Vec2 posA = m_cells[idxA].body->GetPosition();
        b2Vec2 posB = m_cells[idxB].body->GetPosition();

        bool boneToBone       = (m_cells[idxA].type == CT_BONE     && m_cells[idxB].type == CT_BONE);
        bool involvesMuscle   = (m_cells[idxA].type == CT_MUSCLE   || m_cells[idxB].type == CT_MUSCLE);
        bool involvesLigament = (m_cells[idxA].type == CT_LIGAMENT || m_cells[idxB].type == CT_LIGAMENT);

        if (boneToBone) {
            // Weld joint: locks relative position AND angle → bone group acts as one rigid piece
            b2WeldJointDef wjd;
            b2Vec2 midpoint = 0.5f * (posA + posB);
            wjd.Initialize(m_cells[idxA].body, m_cells[idxB].body, midpoint);
            wjd.collideConnected = false;
            wjd.frequencyHz      = 0.0f;  // fully rigid (no rotational spring)
            wjd.dampingRatio     = 0.0f;

            CellWeld cw;
            cw.joint = (b2WeldJoint*)m_world->CreateJoint(&wjd);
            cw.idxA  = idxA;
            cw.idxB  = idxB;
            m_welds.push_back(cw);
            return;
        }

        b2DistanceJointDef djd;
        djd.Initialize(m_cells[idxA].body, m_cells[idxB].body, posA, posB);
        djd.collideConnected = false;

        if (involvesLigament) {
            djd.frequencyHz  = 3.0f;  // soft / elastic
            djd.dampingRatio = 0.5f;
        } else {
            djd.frequencyHz  = 8.0f;  // stiff spring (bone–muscle, muscle–muscle)
            djd.dampingRatio = 0.4f;
        }

        CellJoint cj;
        cj.joint        = (b2DistanceJoint*)m_world->CreateJoint(&djd);
        cj.idxA         = idxA;
        cj.idxB         = idxB;
        cj.isMuscle     = involvesMuscle;
        cj.relaxedLen   = djd.length;
        cj.contractedLen = djd.length * (1.0f - CONTRACT_RATIO);
        m_joints.push_back(cj);
    }

    // ------------------------------------------------------------------ //
    //  Simulation controls
    // ------------------------------------------------------------------ //

    // Convert all cells from static → dynamic and start the simulation
    void startSimulation() {
        if (m_cells.empty()) return;
        for (auto& c : m_cells)
            c.body->SetType(b2_dynamicBody);
        m_mode              = BM_SIMULATING;
        m_musclesContracted = false;
    }

    // Toggle all muscle joints between contracted and relaxed rest lengths
    void toggleMuscles() {
        m_musclesContracted = !m_musclesContracted;
        for (auto& cj : m_joints) {
            if (cj.isMuscle) {
                cj.joint->SetLength(
                    m_musclesContracted ? cj.contractedLen : cj.relaxedLen);
            }
        }
    }

    // ------------------------------------------------------------------ //
    //  Input
    // ------------------------------------------------------------------ //

    void MouseDown(const b2Vec2& p) {
        m_mouseWorld = p;

        if (m_mode == BM_BUILDING) {
            placeCell(gRound(p.x), gRound(p.y));
            return;
        }

        // Simulate mode: standard mouse-joint grab
        if (m_mouseJoint != NULL) return;

        b2AABB aabb;
        b2Vec2 d(0.001f, 0.001f);
        aabb.lowerBound = p - d;
        aabb.upperBound = p + d;

        CellQueryCallback cb(p);
        m_world->QueryAABB(&cb, aabb);

        if (cb.m_fixture && activePower == GRAB) {
            b2Body* body = cb.m_fixture->GetBody();
            b2MouseJointDef md;
            md.bodyA    = m_groundBody;   // base-class static anchor body
            md.bodyB    = body;
            md.target   = p;
            md.maxForce = 1000.0f * body->GetMass();
            m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
            body->SetAwake(true);
        }
    }

    void MouseUp(const b2Vec2& p) {
        World::MouseUp(p);
    }

    void Keyboard(unsigned char key) {
        switch (key) {
        case '1': m_selectedType = CT_BONE;     m_eraseMode = false; break;
        case '2': m_selectedType = CT_LIGAMENT; m_eraseMode = false; break;
        case '3': m_selectedType = CT_MUSCLE;   m_eraseMode = false; break;
        case 'x': case 'X':
            m_eraseMode = !m_eraseMode;
            break;
        case 'g': case 'G':
            if (m_mode == BM_BUILDING && !m_cells.empty())
                startSimulation();
            break;
        case 'c': case 'C':
            clearCreature();
            break;
        case ' ':
            if (m_mode == BM_SIMULATING)
                toggleMuscles();
            break;
        }
    }

    // ------------------------------------------------------------------ //
    //  Step — called every frame
    // ------------------------------------------------------------------ //

    void Step(Settings* settings) {

        // Set an initial view centre that shows the floor and building area
        if (m_firstStep) {
            settings->viewCenter.Set(0.0f, 3.0f);
            m_firstStep = false;
        }

        // Rigid bone–bone joints (frequencyHz=0) need more velocity iterations
        // to fully resolve — boost from the default 8 to 20 in simulate mode.
        if (m_mode == BM_SIMULATING)
            settings->velocityIterations = 20;

        // --- HUD text (drawn before World::Step, same pattern as other worlds) ---
        m_debugDraw.DrawString(10, m_textLine, "Cells & Bodies  |  %s",
            m_mode == BM_BUILDING ? "BUILD MODE" : "SIMULATE MODE");
        m_textLine += 15;

        if (m_mode == BM_BUILDING) {
            const char* typeName = (m_selectedType == CT_BONE)     ? "Bone"     :
                                   (m_selectedType == CT_LIGAMENT)  ? "Ligament" : "Muscle";
            m_debugDraw.DrawString(10, m_textLine,
                "[1]Bone  [2]Ligament  [3]Muscle   Active: %s%s",
                typeName, m_eraseMode ? "  [X] ERASE" : "");
            m_textLine += 15;
            m_debugDraw.DrawString(10, m_textLine,
                "[G] Start simulation   [C] Clear   Cells: %d  Joints: %d",
                (int)m_cells.size(), (int)m_joints.size());
            m_textLine += 15;
        } else {
            m_debugDraw.DrawString(10, m_textLine,
                "[Space] Muscles: %s   [C] Clear & rebuild",
                m_musclesContracted ? "CONTRACTED" : "relaxed");
            m_textLine += 15;
        }

        // --- Background grid in build mode (drawn under everything else) ---
        if (m_mode == BM_BUILDING)
            drawGrid();

        // --- Ghost cell preview under mouse in build mode ---
        if (m_mode == BM_BUILDING)
            drawGhostCell();

        // --- Physics step + Box2D debug draw ---
        World::Step(settings);

        // --- Colour overlays on top of Box2D debug shapes ---
        drawCellOverlays();
    }

    static World* Create() { return new CellsAndBodies; }

private:

    // ------------------------------------------------------------------ //
    //  Visual helpers  (called while GL projection is in world coordinates)
    // ------------------------------------------------------------------ //

    void drawGrid() {
        const float32 h    = WORLD_HALF;
        const float32 yMin = -5.0f;

        glColor4f(0.18f, 0.28f, 0.42f, 1.0f);
        glLineWidth(0.5f);
        glBegin(GL_LINES);
        for (float32 x = -h; x <= h + 0.01f; x += 1.0f) {
            glVertex2f(x, yMin);
            glVertex2f(x, h);
        }
        for (float32 y = yMin; y <= h + 0.01f; y += 1.0f) {
            glVertex2f(-h, y);
            glVertex2f( h, y);
        }
        glEnd();
        glLineWidth(1.0f);
    }

    void drawGhostCell() {
        int gx = gRound(m_mouseWorld.x);
        int gy = gRound(m_mouseWorld.y);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (m_eraseMode) {
            glColor4f(1.0f, 0.15f, 0.15f, 0.40f);
        } else {
            switch (m_selectedType) {
            case CT_BONE:     glColor4f(0.85f, 0.85f, 0.85f, 0.35f); break;
            case CT_LIGAMENT: glColor4f(0.95f, 0.85f, 0.15f, 0.35f); break;
            case CT_MUSCLE:   glColor4f(0.90f, 0.20f, 0.20f, 0.35f); break;
            }
        }
        drawCircle((float32)gx, (float32)gy, CELL_RADIUS, 20);
        glDisable(GL_BLEND);
    }

    // Draw a solid-colour disc over each cell so they're easily identified
    void drawCellOverlays() {
        if (m_cells.empty()) return;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (int i = 0; i < (int)m_cells.size(); i++) {
            b2Vec2 pos = m_cells[i].body->GetPosition();
            switch (m_cells[i].type) {
            case CT_BONE:
                glColor4f(0.80f, 0.80f, 0.80f, 0.65f);
                break;
            case CT_LIGAMENT:
                glColor4f(0.95f, 0.85f, 0.10f, 0.65f);
                break;
            case CT_MUSCLE:
                if (m_musclesContracted)
                    glColor4f(1.00f, 0.08f, 0.08f, 0.75f);  // bright red when firing
                else
                    glColor4f(0.75f, 0.20f, 0.20f, 0.65f);  // dark red when relaxed
                break;
            }
            drawCircle(pos.x, pos.y, CELL_RADIUS * 0.82f, 16);
        }

        glDisable(GL_BLEND);
    }

    static void drawCircle(float32 cx, float32 cy, float32 r, int segs) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= segs; i++) {
            float32 a = (float32)i / (float32)segs * 2.0f * b2_pi;
            glVertex2f(cx + cosf(a) * r, cy + sinf(a) * r);
        }
        glEnd();
    }
};

// Out-of-class definitions for static const members
const float32 CellsAndBodies::WORLD_HALF     = 20.0f;
const float32 CellsAndBodies::CELL_RADIUS    = 0.45f;
const float32 CellsAndBodies::CONTRACT_RATIO = 0.30f;  // muscles contract to 70% of rest length

#endif
