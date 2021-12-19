# 撒点法求面积权重
## 环境
- spreading.C 文件中使用ROOT来生成随机数，以及进行画图（可选）。可以通过更换随机数产生器和剔除绘图代码为纯C++代码。  
- GetWeight.cpp 为纯c++代码，用以实现从csv表格中读取数据
- GetWeight.py 为python3代码，用以实现从csv表格中读取数据

## 文件说明
### spreading.C 
用以生成权重列表的csv文件。其实现思路为：  
1. 遍历可能的圆柱半径（从1mm到100mm），遍历可能的中心点位置（00-99，坐标索引）
2. 以某中心点，某半径撒若干点。并统计所有单元格内点的数目
3. 根据公式： $$weight = \frac{\text { counts }}{\text { Total }} \times \pi r^{2} / S_{\text {cell }}$$ 计算出每个单元格的权重。  
注意，由于撒点法是近似，故认为算出来的权重超过0.99，就都设置为1。且seed中心点的权重一定为1。
4. 根据所得权重值，生成csv文档。其中第一行是表头，前两列是中心cell的坐标索引。

### GetWeight.cpp
读取 spreading.C 生成的csv文档，并返回所查询的权重列表。查询中心cell为CellID，圆柱半径为radius时的这100个cell的权重值。  
 - 函数原型：void GetWeight(double datalist[],int cellID, int radius);
 - dataList[fNofcell] 传入的数组指针  
 - cellID 待查询中心点的位置  
 - raidus 待查询的半径大小，单位mm，为int

### GetWeight.py
读取 spreading.C 生成的csv文档，并返回所查询的权重列表。 查询中心cell为CellID，圆柱半径为radius时的这100个cell的权重值。
 - 函数原型：GetWeight(cellID, radius)
 - cellID 待查询中心点的位置  
 - raidus 待查询的半径大小，单位mm，为int