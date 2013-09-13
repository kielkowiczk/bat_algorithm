/*
 * utils.h
 *
 *  Created on: 13-07-2013
 *      Author: kazek
 */
#include <algorithm>
#include <cfloat>
#include <vector>
#include <cmath>
#include <time.h>

#ifndef UTILS_H_
#define UTILS_H_

/*utility functions*/
namespace utils {
	template<class rand_type>
	rand_type rand_in(rand_type max, rand_type min) {

		return min + (rand_type)rand()/((rand_type)RAND_MAX/(max-min));
	}

	class print_bat {
		public:
			void operator() (float i) { std::cout << " " << i; }
	};

	class print_fit {
	public:
		void operator() (float i) {std::cout << i << std::endl;}
	};

	class print_population {
	public:
		void operator()(std::vector<float> &bat) { std::for_each (bat.begin(), bat.end(), print_bat()); std::cout << std::endl; }
	};

	void display_population_with_fit(const bats_t &population, const bats_fit_t &fit){

		int tmp=0;
		std::for_each(population.begin(), population.end(), [&fit, &tmp](bat_t bat) {
			std::for_each(bat.begin(), bat.end(),  [](float i) { std::cout << i << " ";});
			std::cout << "fit: " << fit[tmp++];
			std::cout << std::endl;
		});
	}

	void display_velocities(const std::vector<bat_t> &v) {
		std::for_each(v.begin(), v.end(), [](bat_t vb){ std::for_each(vb.begin(), vb.end(), [](float i){std::cout<<" " << i;}); std::cout << std::endl;});
	}

	bat_t return_best_bat(bats_t population, bats_fit_t fit) {
		bats_fit_t::iterator fit_it = std::min_element(fit.begin(), fit.end());
		uint idx = fit_it - fit.begin();

		return population[idx];
	}
	void display_v(bat_t bat) {
			std::for_each(bat.begin(), bat.end(), print_bat());
			std::cout <<  std::endl;
		}


	void display_bat(bat_t bat, float fit) {
		std::for_each(bat.begin(), bat.end(), print_bat());
		std::cout << " with fit: " << fit <<  std::endl;
	}

	namespace functions {
		float sphere(bat_t &bat) {
				fit_t  fit=0;
				std::for_each(bat.begin(), bat.end(), [&fit](fit_t &i) {fit+=pow(i,2.0);});
				return fit;
		}

		float rastrigin(bat_t &bat) {
			fit_t fit = 0.0;
			float An = 10.0*bat.size();
			fit+=An;
			std::for_each(bat.begin(), bat.end(), [&](fit_t &x) {fit+=pow(x,2.0)-10*cos(2*3.14159265*x); });
			return fit;
		}

	}
}


#endif /* UTILS_H_ */
