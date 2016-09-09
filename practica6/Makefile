file = tomografia
#file = money

$(file): $(file).pl
	swipl -quiet -O -g main --stand_alone=true -o $(file) -c $(file).pl


clean:
	rm $(file)
