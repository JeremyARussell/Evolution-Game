#ifndef GENETIC_ALGORITHM
#define GENETIC_ALGORITHM

#include <vector>
#include <ctime>


class OldGeneticAlgorithm {
public:
    const static int32 CHROM_SIZE = 40;
    const static int32 VECTOR_NUM = 8;
    float32 MIN_WHEEL;// = 0.1f;
    float32 MAX_WHEEL;// = 1.5f;
    float32 MIN_CART;// = 0.1f;
    float32 MAX_CART;// = 3.0f;
    float32 oldAvg;
    float32 oldMax;
    float32 maxScore;
    float32 avgScore;
    float32 scoreNum;
    float32 fastest_time;
    float32 globalMax;
    int32 popSize;
    vector< vector< float32> > chroms;
    vector< vector< float32 > > bestChrom;
    vector< vector< int32 > > parents;
    vector< vector< float32 > > oldChroms;
    vector< vector< float32 > > newChroms;
    int32 newChromNum;
    vector<float32> scores;
    vector<float32> nextScores;
    vector<int32> votes;
    vector<float32> speed;
    vector<float32> nextSpeed;

    OldGeneticAlgorithm(/*int32 p_popSize,int p_wheelProb,int maxWheels*/) {
		srand(time(0));
		MIN_WHEEL = 0.1f;//Wheel radius or diameter
		MAX_WHEEL = 1.5f;
		MIN_CART= 0.1f;
		MAX_CART = 3.0f;
        m_wheelProb = 0.45f;//p_wheelProb;
        m_wheelNum = 4;//maxWheels;
        popSize = 20;//p_popSize;
        maxScore = 0;
        avgScore = 0;
        oldMax = 0;
        oldAvg = 0;
        scoreNum = 0;
        fastest_time = 9999.99f;
        globalMax = 0;
        int32 chromIndex = 0;
        while (chromIndex < popSize) {
            initChrom(chromIndex);
            chromIndex++;
        }
        return;
    }

    void resetScores() {
        avgScore = 0;
        maxScore = 0;
        scoreNum = 0;
        return;
    }

    void seedWithChrom(vector<vector<float32>> chromToUse) {
        int32 _loc_3 = 0;
        int32 _loc_4 = 0;
        int32 _loc_5 = 0;

        int32 _loc_6 = floor(chromToUse.size() / popSize);//TODO - See if this is right at all, very possibly didn't fix correctly
        newChromNum = 0;
        _loc_5 = 0;
        while (_loc_5 < _loc_6) {
            _loc_3 = 0;
            while (_loc_3 < CHROM_SIZE) {
                chroms[_loc_5][_loc_3] = chromToUse[_loc_5][_loc_3];
                newChroms[_loc_5][_loc_3] = chromToUse[_loc_5][_loc_3];
                _loc_3++;
            }
            _loc_5++;
        }
        int32 _loc_7 = popSize - _loc_6;
        newChromNum = _loc_6;
        _loc_3 = 0;
        while (_loc_3 < _loc_7) {
            crossoverOne(_loc_3 % _loc_6, _loc_3 + _loc_6);
            _loc_3++;
        }
        _loc_3 = 0;
        while (_loc_3 < popSize) {
            _loc_4 = 0;
            while (_loc_4 < CHROM_SIZE) {
                oldChroms[_loc_3][_loc_4] = chroms[_loc_3][_loc_4];
                chroms[_loc_3][_loc_4] = newChroms[_loc_3][_loc_4];
                _loc_4++;
            }
            votes[_loc_3] = 0;
            _loc_3++;
        }
        maxScore = 0;
        avgScore = 0;
        scoreNum = 0;
        return;
    }

    float32 getNextSpeed(int32 param1) {
        return nextSpeed[param1];
    }

    float32 getScore(int32 param1) {
        return scores[param1];
    }

    float32 getNextScore(int32 param1) {
        return nextScores[param1];
    }

    float32 getCartAngle(int param1,int param2) {
        return chroms[param1][param2 * 2];
    }

    float32 getCartMagnitude(int param1,int param2) {
        return chroms[param1][param2 * 2 + 1];
    }

    int32 getWheelOn(int param1,int param2) {
        return (int32)chroms[param1][16 + param2 * 3];
    }

    float32 getAxleAngle(int param1, int param2) {
        return chroms[param1][16 + param2 * 3 + 1];
    }

    float32 getWheelRadius(int param1, int spokeIndex) {
        return chroms[param1][16 + spokeIndex * 3 + 2];
    }

	float32 getChrom(int32 param1, int32 param2, bool param3) {
        if (!param3) {
            return chroms[param1][param2];
        }
        return oldChroms[param1][param2];
    }

    vector<float32> getBestChrom(int32 param1) {
        return bestChrom[param1];
    }

    int32 getVote(int32 param1) {
        return votes[param1];
    }

    float32 getMaxScore() {
        return oldMax;
    }

    float32 getGlobalMax() {
        return globalMax;
    }

    void setScore(int32 index_to_set, float32 score_to_set, float32 speed_to_set) {//TODO - Prepare to remove later
        int32 index_var;
        scores[index_to_set] = score_to_set;
        speed[index_to_set] = speed_to_set;
        if (score_to_set > maxScore) {
            maxScore = score_to_set;
        }
        if (score_to_set > globalMax || score_to_set == globalMax && speed_to_set < fastest_time) {
            globalMax = score_to_set;
            fastest_time = speed_to_set;
            index_var = 0;
            while (index_var < CHROM_SIZE) {
                bestChrom[index_var] = chroms[index_var];
                index_var++;
            }
        }
        avgScore = (avgScore * scoreNum + score_to_set) / (scoreNum + 1);
        float32 _loc_6 = scoreNum + 1;
        scoreNum = _loc_6;
        return;
    }

    void setVote(int32 param1, int32 param2) {//Removal
        votes[param1] = param2;
        return;
    }

    void setSpeed(float32 param1, float32 param2) {//Removal
        speed[param1] = param2;
        return;
    }

    void setChrom(int32 param1, int32 param2, float32 param3) {//Removal, due to not being static, I'll need to remove all the chroms stuff
        chroms[param1][param2] = param3;
        return;
    }

    void initChrom(int chromNum) {//Will be able to remove the param since we won't have multiple chromosome sets per creature
        int geneIndex = 0;//Used to maintain a place for which gene(float32) will be generated
		vector<float32> fresh;
		vector<int32> freshin;
		chroms.push_back(fresh);
        newChroms.push_back(fresh);
		oldChroms.push_back (fresh);
        parents.push_back(freshin);
        scores.push_back(-1);
        nextScores.push_back(-1);
		nextSpeed.push_back(0);
		votes.push_back(0);
		speed.push_back(0);

		geneIndex = 0;//Build body of cart
		chroms[chromNum].resize(40);
        while (geneIndex <= 7) {        
            chroms[chromNum][geneIndex * 2] = randomNumber(0.05f, 1.0f);//Angles, maybe
            chroms[chromNum][geneIndex * 2 + 1] = randomNumber(MIN_CART, MAX_CART);//Length of spokes
            geneIndex++;
        }
        geneIndex = 0;//Build wheel stuff in cart 
        while (geneIndex <= 7) { 
            chroms[chromNum][16 + geneIndex * 3] = randomNumber(0.01f, 1.0f) < m_wheelProb ? (randomNumberInt(0, (VECTOR_NUM - 1))) : (-1);//Wheel divying
            chroms[chromNum][16 + geneIndex * 3 + 1] = randomNumber(0, 5 * b2_pi);
            chroms[chromNum][16 + geneIndex * 3 + 2] = randomNumber(MIN_WHEEL, MAX_WHEEL);//Size of wheels
            geneIndex++;
        }
		parents[chromNum].resize(2);
        parents[chromNum][0] = chromNum;
        parents[chromNum][1] = chromNum;
        return;
    }

    void update(float32 param1, float32 param4,int32 param5,int32 param6,bool using_elite,bool using_tourny) {
        int32 _loc_10 = 0;
        int32 _loc_11 = 0;
        eliteSelection = using_elite;
        oldMax = maxScore;
        oldAvg = avgScore;
        float32 _loc_12 = 0;
        scoreNum = 0;
        maxScore = _loc_12;
        avgScore = _loc_12;
        m_wheelNum = param5;
        m_wheelProb = param6;
        newChromNum = 0;
        if (GetPoolSize() < 2) {
            ReplacePool();
        }
        tournamentSelection();
        _loc_10 = 0;
        while (_loc_10 < popSize) {
            _loc_11 = 0;
            while (_loc_11 < CHROM_SIZE) {
                oldChroms[_loc_10][_loc_11] = chroms[_loc_10][_loc_11];
                chroms[_loc_10][_loc_11] = newChroms[_loc_10][_loc_11];
                _loc_11++;
            }
            votes[_loc_10] = 0;
            _loc_10 = _loc_10 + 1;
        }
        mutation(param4);
        return;
    }

    vector<int32> GetParent(int param1) {
        return parents[param1];
    }

    void crossover(int param1, int param2) {
        if (param1 == param2) {
            nextScores[newChromNum] = scores[param1];
            nextScores[(newChromNum + 1)] = scores[param2];
            nextSpeed[newChromNum] = speed[param1];
            nextSpeed[(newChromNum + 1)] = speed[param2];
        } else {
            nextScores[newChromNum] = -1;
            nextScores[(newChromNum + 1)] = -1;
        }
        int32 _loc_3 = 16 + 3 * m_wheelNum;
        int32 _loc_4 = floor(rand() * _loc_3);
        int32 _loc_5 = floor(rand() * _loc_3);
        int32 _loc_6 = min(_loc_4, _loc_5);
        int32 _loc_7 = max(_loc_4, _loc_5);
        int32 _loc_10 = param1;
        parents[(newChromNum + 1)][0] = param1;
        parents[newChromNum][0] = _loc_10;
        _loc_10 = param2;
        parents[(newChromNum + 1)][1] = param2;
        parents[newChromNum][1] = _loc_10;
        newChromNum = newChromNum + 2;
        return;
    }// end function

    void crossoverOne(int param1, int param2) {
        if (param1 == param2) {
            nextScores[newChromNum] = scores[param1];
            nextSpeed[newChromNum] = speed[param1];
        } else {
            nextScores[newChromNum] = -1;
        }
        int32 _loc_3 = 16 + 3 * m_wheelNum;
        int32 _loc_4 = floor(rand() * _loc_3);
        int32 _loc_5 = floor(rand() * _loc_3);
        int32 _loc_6 = min(_loc_4, _loc_5);
        int32 _loc_7 = max(_loc_4, _loc_5);
        parents[newChromNum][0] = param1;
        parents[newChromNum][1] = param2;
        int32 _loc_11 = newChromNum + 1;
        newChromNum = _loc_11;
        return;
    }

    void copyToNewChroms(int32 num_to_copy) {
        nextScores[newChromNum] = scores[num_to_copy];
        nextSpeed[newChromNum] = speed[num_to_copy];
        int32 _loc_3 = num_to_copy;
        parents[newChromNum][1] = num_to_copy;
        parents[newChromNum][0] = _loc_3;
        int32 _loc_4 = newChromNum + 1;
        newChromNum = _loc_4;
        return;
    }

    void mutation(float32 param1) {
        int32 _loc_2 = 0;
        int32 _loc_3 = 0;
        int32 _loc_4 = 0;
        if (eliteSelection) {
            _loc_4 = 2;
        }
        _loc_2 = _loc_4;
        while (_loc_2 < popSize) {
            _loc_3 = 0;
            while (_loc_3 < CHROM_SIZE) {
                if (rand() < param1) {
                    chroms[_loc_2][_loc_3] = mutateNumber(chroms[_loc_2][_loc_3], _loc_3);
                }
                _loc_3++;
            }
            _loc_2++;
        }
        return;
    }



    float32 mutateNumber(float32 param1, int param2) {
        if (param2 <= 15) {
            if (param2 % 2 == 0) {
                return randomNormalApprox(param1, 0.05f, 1);
            }
            return randomNormalApprox(param1, MIN_CART, MAX_CART);
        } else {
            if ((param2 - 16) % 3 == 0) {
                return rand() < m_wheelProb ? (intPush(param1, VECTOR_NUM)) : (-1);
            }
            if ((param2 - 16) % 3 == 1) {
                return randomNormalApprox(param1, 0, b2_pi * 2);
            }
        }
        return randomNormalApprox(param1, MIN_WHEEL, MAX_WHEEL);
    }

    void ReplacePool() {
        int memberToReplaceIndex = 0;
        memberToReplaceIndex = 0;
        while (memberToReplaceIndex < popSize) {
            if (votes[memberToReplaceIndex] < 0) {
                votes[memberToReplaceIndex] = 0;
                initChrom(memberToReplaceIndex);
            }
            memberToReplaceIndex++;
        }
        return;
    }

    int32 GetPoolSize() {
        int32 _loc_1;
        int32 _loc_2 = 0;
        _loc_1 = 0;
        while (_loc_1 < popSize) {
            _loc_2 = _loc_2 + (votes[_loc_1] >= 0 ? (1) : (0));
            _loc_1 = _loc_1 + 1;
        }
        return _loc_2;
    }

private:

    bool eliteSelection;
    int32 m_wheelNum;
    float32 m_wheelProb;

    vector<float32> topTwo() {
        int32 _loc_7;
        float32 _loc_1 = -1;
        float32 _loc_2 = -1;
        float32 _loc_3 = -1;
        float32 _loc_4 = -1;
        float32 _loc_5 = 9999.99f;
        float32 _loc_6 = 9999.99f;
        _loc_7 = 0;
        while (_loc_7 < popSize) {
            if (scores[_loc_7] > _loc_1 || scores[_loc_7] == _loc_1 && speed[_loc_7] < _loc_5) {
                _loc_1 = scores[_loc_7];
                _loc_2 = (float32)_loc_7;
                _loc_5 = speed[_loc_7];
            }
            _loc_7 = _loc_7 + 1;
        }
        _loc_7 = 0;
        while (_loc_7 < popSize) {
            if (_loc_2 != _loc_7 && (scores[_loc_7] > _loc_4 || scores[_loc_7] == _loc_4 && speed[_loc_7] < _loc_6)) {
                _loc_4 = scores[_loc_7];
                _loc_3 = (float32)_loc_7;
                _loc_6 = speed[_loc_7];
            }
            _loc_7 = _loc_7 + 1;
        }

		vector<float32> fresh_vec;
		fresh_vec.push_back(_loc_2);
		fresh_vec.push_back(_loc_3);


		return fresh_vec;
        //return new (_loc_2, _loc_3);
    }


	//This one is saved, lol. Used in the tournement function above. Will need to fix
	//
	//

    vector<int> removeDuplicates(vector<int> vectorWithDuplicates) {//As the name suggests this function goes through a vector and
        int curDupIndex = 0;										//removes all the duplicate entries.
        int dupSizeIndex = 0;
        int _loc_4 = -1;
        int curNonDupIndex = 0;
        vector<int> dupRemovedVector;
        curDupIndex = 0;
        while (curDupIndex < vectorWithDuplicates.size()) {
            _loc_4 = -1;
            dupSizeIndex = 0;
            while (dupSizeIndex < curNonDupIndex) {
                if (vectorWithDuplicates[curDupIndex] == dupRemovedVector[dupSizeIndex]) {
                    _loc_4 = dupSizeIndex;
                }
                dupSizeIndex++;
            }
            if (_loc_4 == -1) {
                dupRemovedVector[curNonDupIndex] = vectorWithDuplicates[curDupIndex];
                curNonDupIndex++;
            }
            curDupIndex++;
        }
        return dupRemovedVector;
    }

    void tournamentSelection() {
        int32 loop_counter_1 = 0;
        int32 contestant_1 = 0;
        int32 contestant_2 = 0;
		vector<float32> _loc_7;
        vector<int> _loc_4;
        int32 _loc_5 = 0;
        while (_loc_5 < popSize * 2) {
            contestant_1 = selectFromMatingPoolWithoutRepetition(-1);
            contestant_2 = selectFromMatingPoolWithoutRepetition(contestant_1);
            _loc_4[_loc_5] = Tournament(contestant_1, contestant_2);
            _loc_5 = _loc_5 + 1;
        }
        if (eliteSelection) {
            _loc_7 = topTwo();
            copyToNewChroms(_loc_7[0]);
            copyToNewChroms(_loc_7[1]);
        }
        vector<int> _loc_6 = removeDuplicates(_loc_4);//TODO - see what's up with this. Come back and finish later.
        _loc_5 = _loc_6.size();
        if (_loc_5 == 1) {
            loop_counter_1 = 0;
            while (loop_counter_1 < popSize / 2 - (eliteSelection ? (1) : (0))) {
                copyToNewChroms(_loc_6[0]);
                copyToNewChroms(_loc_6[0]);
                loop_counter_1++;
            }
        } else {
            loop_counter_1 = 0;
            while (loop_counter_1 < popSize / 2 - (eliteSelection ? (1) : (0))) {
                contestant_1 = selectWithoutRepetition(-1, _loc_5);
                contestant_2 = selectWithoutRepetition(contestant_1, _loc_5);
                crossover(_loc_6[contestant_1], _loc_6[contestant_2]);
                loop_counter_1++;
            }
        }
        return;
    }//Fixing right now

    int32 Tournament(int contestant_1, int contestant_2) {
        if (votes[contestant_1] > 0 && votes[contestant_2] > 0 || votes[contestant_1] == 0 && votes[contestant_2] == 0) {
            if (scores[contestant_1] > scores[contestant_2]) {
                return contestant_1;
            }
            if (scores[contestant_1] < scores[contestant_2]) {
                return contestant_2;
            }
            if (speed[contestant_1] < speed[contestant_2]) {
                return contestant_1;
            }
            if (speed[contestant_2] < speed[contestant_1]) {
                return contestant_2;
            }
            return rand() > 0.5 ? (contestant_1) : (contestant_2);
        } else {
            if (votes[contestant_1] > 0) {
                return contestant_1;
            }
            return contestant_2;
        }
    }

    bool intPush(int32 param1, int32 param2) {//Fixed this while I was here, also maybe lol
        float32 _loc_3 = (float32)rand();
        if (param1 < 0) {
            return randomNumberInt(0, (VECTOR_NUM - 1));
        }
        return _loc_3 > 0.5 ? ((param1 + 1) % param2) : ((param1 - 1) % param2);
    }

    int32 selectWithoutRepetition(int32 p_not_1, int32 ran_mul) { //FIXED
        int32 retVar;
        do {    
            retVar = (int32)floor(rand() * ran_mul);
        } while (retVar == p_not_1);
        return retVar;
    }

    int32 selectFromMatingPoolWithoutRepetition(int32 param1) {
        int32 _loc_2 = 0;
        int32 pool_size = 0;
        int32 _loc_5 = 0;
        int32 _loc_6 = 0;
        int32 _loc_4 = -1;
        pool_size = GetPoolSize();
        do {   
            _loc_5 = (int32)floor(rand() * pool_size);
            _loc_6 = -1;
            _loc_2 = 0;
			while (_loc_2 < popSize) {
                _loc_6 = _loc_6 + (votes[_loc_2] >= 0 ? (1) : (0));
                if (_loc_6 == _loc_5) {
                    _loc_4 = _loc_2;
                    break;
                }
                _loc_2 = _loc_2 + 1;
            }
        } while (_loc_4 == param1);
        return _loc_4;
    }

	

    float32 randomNumber(float32 low = 0, float32 high = 1) {
		float retNum;
		retNum = low + (float)rand() / ((float)RAND_MAX / (high-low));
        return retNum;
    }

    float32 randomNormalApprox(float32 param1 = 0.5, float32 param2 = 0, float32 param3 = 1) {
        float32 _loc_6;
        float32 _loc_7;
        float32 _loc_4 = (float32)rand();
        float32 _loc_5 = (param3 - param2) / 6;
        if (_loc_4 < 0.023) {
            _loc_6 = max(param1 - 3 * _loc_5, param2);
            _loc_7 = max(param1 - 2 * _loc_5, param2);
        }
			else if (_loc_4 < 0.159) {
				_loc_6 = max(param1 - 2 * _loc_5, param2);
				_loc_7 = max(param1 - _loc_5, param2);
			}
				else if (_loc_4 < 0.5) {
					_loc_6 = max(param1 - _loc_5, param2);
					_loc_7 = param1;
				}
					else if (_loc_4 < 0.841) {
						_loc_6 = param1;
						_loc_7 = min(param1 + _loc_5, param3);
					}
						else if (_loc_4 < 0.977) {
							_loc_6 = min(param1 + _loc_5, param3);
							_loc_7 = min(param1 + 2 * _loc_5, param3);
						} else {
							_loc_6 = min(param1 + 2 * _loc_5, param3);
							_loc_7 = min(param1 + 3 * _loc_5, param3);
						}
        return randomNumber(_loc_6, _loc_7);
    }

    int32 randomNumberInt(float32 param1 = 0, float32 param2 = 1) {
        return (int32)floor(randomNumber(param1, (param2 + 1)));
    }
};
#endif
