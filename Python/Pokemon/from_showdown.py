# dict_pokemon = {}

# with open("pokedex.txt", "r") as f:
#     raw_data = f.read()

# new_data = raw_data.split("\t},")

# new_new_data = []
# for d in new_data:
#     new_new_data.append(d.split('\n'))

# for i in range(len(new_new_data)):
#     for j in range(len(new_new_data[i])):
#         new_new_data[i][j] = new_new_data[i][j].replace('\t', '')

# del new_new_data[0][0]

# # for i in range(len(new_new_data)):
# for i in range(1):
#     while '' in new_new_data[i]:
#         new_new_data[i].remove('')

#     del new_new_data[i][0]

#     for j in range(len(new_new_data[i])):
#         new_new_data[i][j] = new_new_data[i][j][:-1]
#         # new_new_data[i][j] = new_new_data[i][j].split(': ')
#         a, b = new_new_data[i][j].split(': ')
#         print(a)
#         print(b)

#         try:
#             dict_pokemon[a] = b

#         except ValueError:
#             pass

    

# print(new_new_data[0])




# # dict_data = []
# # for i in range(len(new_new_data)):
# #     dict_data.append([])
# #     for j in range(len(new_new_data[i])):
# #         dict_data.append()
