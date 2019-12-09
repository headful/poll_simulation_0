#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <string>
#include <sstream>

using std::vector;
using std::pair;
using std::set;
using std::string;
using std::ostream;
using std::istream;

struct poll_config {
	int number_of_iterations = 15;
	int number_of_options = 9;
	int number_of_samples = 500;
	int desired_population_size = 1000000;
	int actual_population_size = 1000000;
	vector<double> real_choice = { 0.30,0.25,0.20,0.15,0.10,0.0,0.0,0.0,0.0 };
	vector<string> candidate_names = { "Biden","Warren","Sanders","Pete B.","Yang","Gabbard","Steyer","Delaney","Bloomberg" };
};
struct poll_domain {
	vector<int> choice_lobound;
	int chosen(int c_idx) {
		for (int i = choice_lobound.size() - 1; i >= 0; --i) {
			if (c_idx >= choice_lobound[i])
				return i;
		}
	}
};
vector<int> spp_options({ 500,700,1000,2000,4000 });
string distr_to_string(vector<double>& vals) {
	if (vals.size() == 0) return "";
	std::stringstream ss;
	ss << vals[0] * 100;
	for (int i = 1; i < vals.size(); ++i) {
		if (vals[i] > 0.0)
			ss << "/" << vals[i] * 100;
	}
	return ss.str();
}
void report(poll_config& p_config, ostream&  out) {
	out << "\n\tCurrent Configuration\n\n";
	out << "number of polls\t\t" << p_config.number_of_iterations << "\n";
	out << "samples per poll\t" << p_config.number_of_samples << "\n";
	out << "population\t\t" << p_config.desired_population_size << "\n";
	out << "vote distr\t\t" << distr_to_string(p_config.real_choice) << "\n";
}
void show_poll_results(ostream& cout, poll_config& p_config, vector<vector<int>>& poll_results) {
	int prec = p_config.number_of_samples >= 2000 ? 2 : 1;
	cout << "\nname    actual       polls....\n";
	cout << "------------------------------------------------------------------------------------------\n";
	for (int i = 0; i < p_config.number_of_options; ++i)
		if (p_config.real_choice[i] > 0.0) {
			cout << std::fixed << std::setprecision(prec) << p_config.candidate_names[i] << "\t" << p_config.real_choice[i] * 100 << "\t";
			for (int j = 0; j < p_config.number_of_iterations; ++j) {
				cout << std::fixed << std::setprecision(prec) <<
					((double)poll_results[j][i] / p_config.number_of_samples) * 100 << "\t";
			}
			cout << "\n";
		}
}

void set_domain(poll_domain& p_domain, poll_config& p_config) {
	vector<int> chosen_ct;
	int sum = 0;
	for (int i = 0; i < p_config.number_of_options; ++i) {
		int contr = (int)(p_config.desired_population_size * p_config.real_choice[i]);
		chosen_ct.push_back(contr);
		sum += contr;
	}
	p_config.actual_population_size = sum;
	p_domain.choice_lobound.clear();
	int ct = 0;
	for (int i = 0; i < p_config.number_of_options; ++i) {
		int lbound = ct;
		int ubound = lbound + chosen_ct[i];
		ct = ubound;
		p_domain.choice_lobound.push_back(lbound);
	}
}


void menu_spp(ostream& out, istream& in) {
	out << "\n\tMenu >> Configuration >> Set samples per poll" << "\n\n";
	for (int i = 0; i < spp_options.size(); ++i)
		out << i + 1 << " -- " << spp_options[i] << "\n";

	out << "b -- back\n";
	out << "\nEnter 1-" << spp_options.size() << " or b:";
}
void menu_vp(ostream& out, poll_config& p_config) {
	out << "\n\n\tMain Menu >> Configuration >> Vote distribution\n\n";
	out << "1 -- 30/25/20/15/10\n";
	out << "2 -- 25/18/15/13/10/9/6/3/1\n";
	out << "3 -- 25/25/25/25\n";
	out << "b -- back\n";
	out << "\nEnter 1-3 or b:";
}
void menu_config(ostream& out, poll_config& p_config, poll_domain& p_domain) {
	out << "\n\n\tMain Menu >> Configuration\n\n";
	out << "1 -- number of samples per poll  (" << p_config.number_of_samples << ")\n";
	out << "2 -- set votes % per candidate (" << distr_to_string(p_config.real_choice) << ")\n";
	out << "b -- back\n";
	out << "\nEnter 1, 2 or b:";
}
void menu_main(ostream& out) {
	out << "\n\tMain Menu\n";
	out << "\n";
	out << "1 -- simulate polls\n";
	out << "2 -- configure simulation\n";
	out << "x -- exit\n";
	out << "\nEnter 1,2 or x:";
}

void edit_spp(ostream& out, istream& in, poll_config& p_config) {
	menu_spp(out, in);
	char cmd_in[256];
	in.getline(cmd_in, 256);
	if (cmd_in[0] != 'b') {
		int idx = cmd_in[0] - '1';
		if (idx >= 0 && idx < spp_options.size())
			p_config.number_of_samples = spp_options[idx];
	}
}
void edit_vp(ostream& out, istream& in, poll_config& p_config, poll_domain& p_domain) {
	menu_vp(out, p_config);
	char cmd_in[256];
	in.getline(cmd_in, 256);
	if (cmd_in[0] != 'b') {
		switch (cmd_in[0]) {
		case '1':
			p_config.real_choice = { 0.30,0.25,0.20,0.15,0.10,0.0,0.0,0.0,0.0 };
			break;
		case '2':
			p_config.real_choice = { 0.25,0.18,0.15,0.13,0.10,0.09,0.06,0.03,0.01 };
			break;
		case '3':
			p_config.real_choice = { 0.25,0.25,0.25,0.25,0.0,0.0,0.0,0.0,0.0};
			break;
		default:
			break;
		};
	}
}
void edit_config(ostream& out, istream& in, poll_config& p_config, poll_domain& p_domain) {
	menu_config(out, p_config, p_domain);
	char cmd_in[256];
	in.getline(cmd_in, 256);
	if (cmd_in[0] != 'b')
		if (cmd_in[0] == '1') {
			edit_spp(out, in, p_config);
		}
		else if (cmd_in[0] == '2') {
			edit_vp(out, in, p_config, p_domain);
		}
}

vector<int> sim_poll(poll_domain& p_domain, poll_config& p_config
					, std::mt19937& rng, std::uniform_int_distribution<int>& dist) {
	vector<int> result(p_config.number_of_options);
	set<int> selected_idx;

	int sample_count = 0;
	while (sample_count < p_config.number_of_samples) {
		int rndm_idx = dist(rng);
		if (selected_idx.find(rndm_idx) == selected_idx.end()) {
			result[p_domain.chosen(rndm_idx)]++;
			sample_count++;
			selected_idx.insert(rndm_idx);
		}
	}
	return result;
}
void main_loop() {
	poll_domain p_domain;
	poll_config p_config;
	report(p_config, std::cout);
	menu_main(std::cout);
	char cmd_in[256];
	std::cin.getline(cmd_in, 256);

	while (cmd_in[0] != 'x') {
		vector<vector<int>> poll_results;
		switch (cmd_in[0]) {
		case '1':
		{
			set_domain(p_domain, p_config);
			std::random_device rd;
			std::mt19937 rng(rd());
			std::uniform_int_distribution<int> dist(0, p_config.actual_population_size - 1);
			for (int i = 0; i < p_config.number_of_iterations; ++i) {
				vector<int> poll_out = sim_poll(p_domain, p_config, rng, dist);
				poll_results.push_back(poll_out);
			}
		}
			break;
		case '2':
			edit_config(std::cout, std::cin, p_config, p_domain);
			report(p_config, std::cout);
			break;
		default:break;
		}

		if (poll_results.size() > 0)
			show_poll_results(std::cout, p_config, poll_results);
		menu_main(std::cout);
		std::cin.getline(cmd_in, 256);
	}
}
int main() {
	main_loop();
	return 0;
}

