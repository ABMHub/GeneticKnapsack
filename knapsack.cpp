#include <iostream>
#include <vector>
#include <cstdlib>
#include <thread>
#include <mutex>

using namespace std;

mutex new_genes_lock;

struct Item {
  char * nome;
  int peso;
  int valor;  
};

template <typename T>
void print_vec(vector<T> vec) {
  for (int i = 0; i < vec.size(); i++) 
    cout << vec[i] << " ";
  cout << endl;
}

bool rand_bool() {
  return (rand() % 2) == 1;
}

vector<vector<bool>> create_random_genes(int n_pos, int n_genes) {
  vector<vector<bool>> ret;
  for (int i = 0; i < n_genes; i++) {
    vector<bool> gene;
    for (int j = 0; j < n_pos; j++) {
      gene.push_back(rand_bool());
    }
    ret.push_back(gene);
  }
  return ret;
}

void evaluate_sack(int max_wheight, Item item_array[], vector<bool> gene, int * value) {
  int curr_wheight = 0;
  int curr_value = 0;
  for (int i = 0; i < gene.size(); i++) {
    if (gene[i] == true) {
      curr_wheight += item_array[i].peso;
      if (curr_wheight > max_wheight) {
        *value = 0;
        return;
      }
      curr_value += item_array[i].valor;
    }
  }
  *value = curr_value;
}

bool all_zeroes(vector<int> v) {
  for (int i = 0; i < v.size(); i++) 
    if (v[i] != 0) return false;
  
  return true;
}

// possivel que os dois "pais" sejam o mesmo
pair<int, int> get_couple(vector<int> values) {
  if (all_zeroes(values))
    values = vector<int>(values.size(), 1);

  int total = 0;
  for (int i = 0; i < values.size(); i++) {
    total += values[i];
  }

  int couple[2];
  for (int j = 0; j < 2; j++) {
    int rand_num = (rand() % total)+1;
    int k = 0;
    while (rand_num > 0) {
      rand_num -= values[k];
      k++;
    }
    couple[j] = k-1;
  }
  pair<int, int> p = make_pair(couple[0], couple[1]);
  
  return p;
}

pair<vector<bool>, vector<bool>> cross_over(vector<bool> parent1, vector<bool> parent2, int crossover_point) {
  for (int i = crossover_point; i < parent1.size(); i++) {
    auto temp = parent1[i];
    parent1[i] = parent2[i];
    parent2[i] = temp;
  }
  return {parent1, parent2};
}

vector<bool> mutation(vector<bool> gene) {
  int size = gene.size();
  for (int i = 0; i < size; i++) {
    if (rand()%size == 0) {
      gene[i] = !gene[i];
    }
  }
  return gene;
}

void gen_children(vector<int> values, vector<vector<bool>> genes, vector<vector<bool>> * new_genes) {
  cout << "a" << endl;
  int gene_size = genes[0].size();
  auto couple = get_couple(values);
  auto children = cross_over(genes[couple.first], genes[couple.second], rand()%gene_size);
  auto c1 = mutation(children.first);
  auto c2 = mutation(children.second);
  new_genes_lock.lock();
    new_genes->push_back(c1);
    new_genes->push_back(c2);
  new_genes_lock.unlock();
  cout << "b" << endl;

}

void get_top_n_genes(vector<int> values, vector<vector<bool>> genes, vector<vector<bool>> * elite) {
  // ordenacao eh pior que busca sequencial
  pair<int, int> ret_idx;
  int top1 = -1, top2 = -1;
  for (int i = 0; i < values.size(); i++) {
    if (top1 < values[i]) {
      ret_idx = {i, ret_idx.first};

      top2 = top1;
      top1 = values[i];
    }
    else if (top2 < values[i]) {
      ret_idx.second = i;

      top2 = values[i];
    }
  }
  (*elite)[0] = genes[ret_idx.first];
  (*elite)[1] = genes[ret_idx.second];
}

extern "C" {
  void knapsack_problem(Item item_array[], int max_wheight, int item_qtd, int max_iterations) {
    int genes_qtd = 10;
    for (int i = 0; i < item_qtd; i++) {
      cout << item_array[i].nome << endl;
      cout << item_array[i].valor << endl;
      cout << item_array[i].peso << endl << endl;
    }
    vector<vector<bool>> genes = create_random_genes(item_qtd, genes_qtd);
    for (int i = 0; i < genes.size(); i++) {
      print_vec(genes[i]);
    }
    for (int j = 0; j < 100; j++) {

    vector<int> values(genes_qtd, 0);
    vector<thread> t_vec(genes_qtd);
    for (int i = 0; i < genes_qtd; i++) {
      // todo: semaforo para limitar threads
      t_vec[i] = thread(evaluate_sack, max_wheight, item_array, genes[i], &(values[i]));
    }
    for (int i = 0; i < genes_qtd; i++) 
      t_vec[i].join();

    // print_vec(values);
    int children_qtd = 8;
    vector<vector<bool>> new_genes;
    t_vec = vector<thread>(children_qtd);
    
    vector<vector<bool>> elite(2);
    thread elite_t(get_top_n_genes, values, genes, &elite);
    for (int i = 0; i < children_qtd; i++) {
      // gen_children(values, genes, &new_genes);
      t_vec[i] = thread(gen_children, values, genes, &new_genes);
    }

    for (int i = 0; i < children_qtd; i++) 
      t_vec[i].join();
    elite_t.join();

    cout << "d" << endl;

    elite.insert(elite.end(), new_genes.begin(), new_genes.end());
    genes = elite;
    for (int i = 0; i < genes.size(); i++) {
      print_vec(genes[i]);
    }
    cout << endl;
    int total = 0;

    // for (int i =0 ; i < values.size(); i++) {
    //   total += values[i];
    // }
    // cout << total/values.size() << endl;
    }
  }
}