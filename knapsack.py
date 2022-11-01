import ctypes
# import os
# os.add_dll_directory(os.getcwd())
lib = ctypes.CDLL("./knapsack.so")

class Item(ctypes.Structure):
  _fields_ = [("nome", ctypes.c_char_p),
              ("peso", ctypes.c_int),
              ("valor", ctypes.c_int)]

def array_test():
  global lib
  func = lib.knapsack_problem
  num_items = 8
  arr = (Item * num_items)(Item(b"a", 1, 1), Item(b"b", 1, 2), Item(b"b", 1, 4), Item(b"b", 1, 8), Item(b"b", 1, 16), Item(b"b", 1, 32), Item(b"b", 1, 64), Item(b"b", 1, 128))
  # func.argtypes = [ctypes.Array(Item), ctypes.c_int, ctypes.c_int]
  # func.restype = ctypes.POINTER(Item)
  # func.restype = Item
  # print(arr)
  func(arr, ctypes.c_int(int(4)), ctypes.c_int(num_items))
  # print(a.nome)
  # print(a.contents.nome)

array_test()
