# Pocket-Cube

ポケットキューブを A* アルゴリズムで解くプログラムです．
かなり力任せの探索を行うため，高速ではありません．

## 実行方法

例えば，次のようにコンパイルして実行して下さい
~~~
Pocket-Cube$ gcc pocket_cube.c -o <binary> -O3 -lm
Pocket-Cube$ <binary> <rotate> <seed>
~~~

### 出力結果例
~~~
Pocket-Cube$ time ./a.out 14
rotate: 14
[GOAL]->RU->RU->LD->BL->LU->RD->BL->BL->RU->BL->LU->RD->RD->RD->[START]
        [9][H]
        [3][N]
  [I][4][B][1][G][D][M][C]
  [E][A][F][J][8][K][0][6]
        [5][2]
        [L][7]
Loop: 11009/ size: 34172/ node: 45181/ miss: 34084/ extend:    0
steps: 10
        [0][1]
        [3][2]
  [7][4][B][8][J][G][N][K]
  [6][5][A][9][I][H][M][L]
        [F][C]
        [E][D]
[GOAL]<-RD<-BR<-RU<-LD<-RD<-BR<-RD<-BR<-LD<-RD<-[START]
SUCCEED!

real    0m29.324s
user    0m29.308s
sys     0m0.010s
~~~
