for gcut in range(1, 9):
    # Reading gcut old, remove newline and put in a list
    # with open('instances/'+gcut+'1.txt', "r") as f:
    # lines = [line.rstrip() for line in f.readlines()]
    with open('instances/gcut{}.txt'.format(gcut), "r") as f:
    # with open('instances/type1/ngcutfs1_{}.txt'.format(gcut), "r") as f:
        array = [[int(x) for x in line.split()] for line in f]
    # Creating a new file with a prefix adpp
    file2 = open('instances/gcut{}_adpp.txt'.format(gcut), "w")
    # file2 = open('instances/type1/ngcutfs1_{}_adpp.txt'.format(gcut), "w")
    numberItems = array[0][0]
    array[1][0] //= 50
    array[1][1] //= 50

    for item in range(0, numberItems):
        array[item+2][0] =  [array[item+2][0] // 50, 1][(array[item+2][0]// 50) < 1]
        array[item+2][1] =  [array[item+2][1] // 50, 1][(array[item+2][1]// 50) < 1]
        array[item+2][4] =  [array[item+2][4] // 50, 1][(array[item+2][4]// 50) < 1]
        pass
    # print(array[1])
    # print(array)
    file2.write('{}\n'.format(array[0][0]))
    file2.write('{}\t{}\n'.format(array[1][0],array[1][1]))

    for item in range(0, numberItems):
        file2.write('{}\t{}\t{}\t{}\t{}\n'.format(array[item+2][0],array[item+2][1],array[item+2][2],array[item+2][3],array[item+2][4])) 
        pass
    file2.close()


