make
cp s2-6el0 s2-6el
cp s1-14el0 s1-14el
rm s0
clear

echo "./stack_counters #########################################"
./stack_counters
echo "./reader ####################################################"
./reader
echo "./reader s ####################################################"
./reader s
echo
echo "time ./stack_counters s0 # pila inexistente ######################"
time ./stack_counters s0
echo
echo "./reader s0 ####################################################"
./reader s0
echo 
echo "time ./stack_counters s0 # 2ª ejecución misma pila ###############"
time ./stack_counters s0
echo
echo "./reader s0 ####################################################"
./reader s0
echo
echo "./stack_counters s1-14el #####################################"
# pila con 14 elementos inicializados a 0
./stack_counters s1-14el
echo
echo "./reader s1-14el ###############################################"
# los calculos han de hacerse sobre los 10 superiores
./reader s1-14el
echo
echo "./stack_counters s2-6el ####################################################"
#pila con 6 elementos 
./stack_counters s2-6el
echo
echo "./reader s2-6el ####################################################"
./reader s2-6el
