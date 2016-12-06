program SquareRoot

var guess, x, change, oldGuess : integer;

begin
    guess := 1;
    
    { Loop until the change in the guess is small }
    while (change > .001) do
    begin
         oldGuess := guess;
         guess := (x / guess + guess) / 2;
         change := abs(guess - oldGuess);
    end

    print
end.