# Курсовая работа
## Тема: 
cast256
## Сборка

OS: KDE neon User Edition 5.20 x86_64

Kernel: 5.4.0-58-generic

Compiler version: g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0

```
g++ -std=c++17 main.cpp cast256.cpp
```

## Использование

Шифрование

```
./a.out -e ./ key.txt IV.txt enc.txt
```

Расшифрование

```
./a.out -d enc.txt key.txt IV.txt
```
