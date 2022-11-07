import ctypes
import numpy as np
from typing import List
lib = ctypes.CDLL("./knapsack.so")

class C_Item(ctypes.Structure):
  _fields_ = [("nome", ctypes.c_char_p),
              ("peso", ctypes.c_int),
              ("valor", ctypes.c_int)]

class Item:
  def __init__(self, nome : str, peso : int, valor : int):
    self.nome = nome
    self.peso = peso
    self.valor = valor

class Knapsack:
  def __init__(self, items : List[Item], peso_maximo : int = 4):
    self.items = items
    self.peso_maximo = peso_maximo
  
  def __cvt_struct(self) -> List[C_Item]:
    return [C_Item(str.encode(item.nome), item.peso, item.valor) for item in self.items]

  def find_best_combination(self, iteracoes : int, tamanho_populacao : int = 10, elitismo : int = 2, verboso : bool = False) -> List[str]:
    assert (tamanho_populacao % 2) == 0
    assert (elitismo % 2) == 0
    assert tamanho_populacao > elitismo
    
    global lib
    func = lib.knapsack_problem
    num_items = len(self.items)
    arr = (C_Item * num_items)(*self.__cvt_struct())

    # int * knapsack_problem(Item item_array[], int max_wheight, int item_qtd, int max_iterations)
    func.restype = ctypes.POINTER(ctypes.c_int * num_items)
    res = func(arr, ctypes.c_int(int(4)), ctypes.c_int(num_items), ctypes.c_int(iteracoes), ctypes.c_int(tamanho_populacao), ctypes.c_int(elitismo), ctypes.c_bool(verboso))

    # testar ctypes.bool no retorno
    python_res = [bool(elem) for elem in list(res.contents)]
    lib.free_ptr(res)   

    nomes = np.array([item.nome for item in self.items])
    return nomes[python_res]

items = [Item("Um real", 1, 1), Item("Chaveiro", 1, 2), Item("Pacote de Figurinhas", 1, 4), Item("Desodorante", 1, 8), Item("Protetor Solar", 1, 16), Item("Vitamina D", 1, 32), Item("Carteira", 1, 64), Item("Cartão de Crédito", 1, 128)]
ks = Knapsack(items=items)
print(ks.find_best_combination(100))
