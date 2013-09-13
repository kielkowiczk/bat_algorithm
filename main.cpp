#include <iostream>
#include <algorithm>
#include <numeric>
#include <cfloat>
#include <vector>
#include <assert.h>

#include <functional>

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),  std::back_inserter(result), std::plus<T>());
    return result;
}

template <typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::minus<T>());
    return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, const float& b)
{
    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), std::back_inserter(result), [&b](float f){return f*b;});

    return result;
}

/*simulaion parameters*/
//#define POP_SIZE 100
#define BAT_LENGTH 10

//#define Q_MIN 0
//#define Q_MAX 0.1

/*simulation space*/
#define MIN -1.0
#define MAX 1.0

/*custom type definition*/
#define uint unsigned int
#define bat_t std::vector<float>
#define bats_t std::vector<bat_t>
#define fit_t float
#define bats_fit_t std::vector<fit_t>

#include "utils.h"

void init_bat(bat_t &bat, uint len){ for (uint i = 0; i<len; i++) { bat.push_back(utils::rand_in<float>(MIN, MAX)); } }

bat_t init_bat(uint bat_length, float pMIN=MIN, float pMAX=MAX) { bat_t bat; for (uint i=0; i<bat_length; i++ ) { bat.push_back(utils::rand_in<float>(pMIN, pMAX)); } return bat; }

void init_population(bats_t &pop, uint population_length, uint bat_length) { for (uint i=0; i< population_length; i++) { pop.push_back( init_bat(bat_length) ); } }

void init_fit(bats_fit_t &fit, uint length) { for (uint i=0; i<length; i++) { fit.push_back(FLT_MAX); } }

void evaluate_population(bats_fit_t &fit, bats_t &population, fit_t (*fit_fnc)(bat_t &) ) { uint i=0; std::for_each(population.begin(), population.end(), [&i, &fit, &fit_fnc](bat_t &bat) { fit[i++] = fit_fnc(bat); } ); }

namespace BA{
	bat_t algorithm(float A, float r, uint pop_size, uint max_it, fit_t (*fit_fnc)(bat_t &), float q_min=0, float q_max=MAX/10, uint debug=1, std::vector<float> *avg_fit_at_it=NULL, std::vector<float> *best_at_iter=NULL) {
		srand(time(NULL)); /*inits randomness*/

		bats_t population;
		bats_fit_t fit;

		std::vector<float> Q(pop_size); /*frequency*/
		std::vector<bat_t> v(pop_size); /*bats velocity*/

		/*inits bats velocity to zero*/
		std::for_each(v.begin(), v.end(), [](bat_t &velocity) { velocity = init_bat(BAT_LENGTH, 0.0, 0.0); });

		init_population(population, pop_size, BAT_LENGTH);
		init_fit(fit, pop_size);
		uint it = 0;

		do {
		/*one iteration of bat alg.*/
		evaluate_population(fit, population, fit_fnc);
		bat_t best_bat = utils::return_best_bat(population, fit);

		if (debug >=2) {
			best_at_iter->push_back(fit_fnc(best_bat));
			avg_fit_at_it->push_back(std::accumulate(fit.begin(), fit.end(), 0.0) / fit.size());
		}

		uint index = 0;

		std::for_each(population.begin(), population.end(), [&](bat_t &bat) {
			//Q[index] = utils::rand_in<float>(Q_MIN, Q_MAX);
			Q[index] = utils::rand_in<float>(0.0, 0.1);

			bat_t candidate_v = v[index] + (bat - best_bat)*Q[index];
			bat_t candidate_bat = bat + candidate_v;

			if (utils::rand_in<float>(0.0,1.0) > r) {
				/*random walk in direction of candidate_v scalled down by q_max/100 factor*/
				for (int i=0;i<2;i++) {
					candidate_v = init_bat(BAT_LENGTH)*(q_max/100);
					candidate_bat = best_bat+candidate_v;
				}
			}

			/*evaluate candidate solution, it might be either solution found by appling movement equotions to current solution or by using random walk around best solution*/
			float candidate_bat_fit = fit_fnc(candidate_bat);

			/*accept candidate solution as current solution if better or if not better accept solution with some small probability*/
			if (candidate_bat_fit < fit[index] || utils::rand_in<float>(0.0,1.0) < A) {
				bat = candidate_bat;
				v[index] = candidate_v;
				fit[index] = candidate_bat_fit;
			}

			index++;
		});

		if (debug>=1 && it%100==1) {
			evaluate_population(fit, population, fit_fnc);
			bat_t best = utils::return_best_bat(population, fit);
			std::cout << "at iteration: " << it << " pop avg fit: " <<  std::accumulate(fit.begin(), fit.end(), 0.0) / fit.size() << " best bat fit: " << fit_fnc(best) << std::endl;
		}
		}while(++it < max_it);

		/*find and return best solution*/
		evaluate_population(fit, population, fit_fnc);
		return utils::return_best_bat(population, fit);
	}
}

int main() {

	/*simple usage*/
	bat_t solution = BA::algorithm(0.001, 0.999, 1000, 2000, utils::functions::rastrigin);

	/*
	 * usage with avarage fit and best solution in each iteration statistics
	std::vector<float> *avg_fit_iter = new std::vector<float>();
	std::vector<float> *best_iter = new std::vector<float>();
	bat_t solution = BA::algorithm(0.1, 0.95, 100, 1000, utils::functions::sphere, 0, MAX/10, 2, avg_fit_iter, best_iter);

	std::for_each(avg_fit_iter->begin(), avg_fit_iter->end(), [](float f) {std::cout << f <<std::endl;});
	*/

	utils::display_bat(solution, utils::functions::rastrigin(solution));
	return 0;

}
