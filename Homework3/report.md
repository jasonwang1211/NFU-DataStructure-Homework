# 41143109
作業三報告

## Polynomial ADT（循環鏈結串列實作 + Available List 加分項）
---

## 解題說明

### 1.1 題目需求
實作一個完整的 **Polynomial** 類別，使用**帶頭結點的循環鏈結串列**表示單變數多項式，每項係數為整數，指數遞減排列。每個節點包含三個成員：`coef`、`exp`、`link`。

必須實作以下功能：
(a) `istream& operator>>`（讀入 coef exp 成對）  
(b) `ostream& operator<<`（美觀輸出多項式）  
(c) Copy constructor  
(d) Assignment operator (`operator=`)  
(e) Destructor（將所有節點回收至 Available List）  
(f) `operator+`  
(g) `operator-`  
(h) `operator*`  
(i) `float Evaluate(float x)`  

加分項：實作 Available-space list 進行記憶體回收與重用。

### 1.2 範例測試
