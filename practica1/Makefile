all: pau-sat


pau-sat: SAT-pau.cpp
	g++ -o mysat -O3 SAT-pau.cpp

p2: SAT-pau2.cpp
	g++ -o mysat -O3 SAT-pau2.cpp

alum-sat: SAT-alumnes.cpp
	g++ -o alum -O3 SAT-alumnes.cpp

test:
	time picosat < in/vars-100-10.cnf
	
	time mysat < in/vars-100-10.cnf

#qwert:
#    num1=100 ; while [[ $$num1 -le 300 ]] ; do \
        num2=1 ; while [[ $$num2 -le 10 ]] ; do \
            mysat $$num1 $$num2 ; \
            ((num2 = num2 + 1)) ; \
        done ; \
        ((num1 = num1 + 50)) ; \
    done
