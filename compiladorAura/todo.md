tratar rotulos criados no arquivo fonte -> usar R nos que haviam no codigo; usar L nos que criamos pro MEPA

usar o nome do método para os rótulos dele ao invés de apenas o contador de rótulo

## Uma coisa que a gente tinha deixado passar:
Nao estamos guardando os labels de comandos com rotulo, ou seja, estamos aceitando coisas como:

label 100, 100;  (declaracao de dois labels iguais)


(dois comandos com o mesmo rotulo, o que iria dar problema no goto)
100: s:=f(t); t:=s;
    if t<x then goto 100;
100: x:=x-1


vai ter que criar uma natureza "label" na tabela de simbolos e guarda-los




## outra coisa, mas agora do MEPA:
No compilar parametros, a gente tem que devolver a quantidade de parametros para poder poder usar RTPR k, n
Porque n eh a quantidade de parametros 

