#!/usr/bin/python3

# cellID 待查询中心点的位置
# raidus 待查询的半径大小，单位mm，为int
def GetWeight(cellID, radius):
    filename = "WeightResult"+str(radius)+".csv"

    with open(filename, 'r') as f:
        for _ in range(cellID+1):  # +1 是为了把表头去掉
            f.readline()
        line = f.readline()  # 带查询的数据行

    dataStringList = line.split("\t")[2:-1]  # [2:-1] 去掉表示位置的前两列和行尾的\n

    # return [float(x) for x in line]
    return [float(x) for x in dataStringList]


if __name__ == '__main__':
    radius = "100"
    cellID = 55
    print(GetWeight(cellID, radius))
