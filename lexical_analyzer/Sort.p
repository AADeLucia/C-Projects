program Sort

type integerArray = array[1..100] of integer;
var a : integerArray;

{ Our procedures are old-school and operate on global variables }

procedure selectionSort
begin

    { Local variables for loop counters }
    var i, j, minIndex, minValue, temp : integer;
    
    i := 1;
    
    while (i < 100) do
    begin
        minIndex := i;
        minValue := a[i];
        j := i + 1;
        
        { Look for the minimum element in positions i to 100 }
        while (j < 100) do
        begin
            if (a[j] < minValue) then
            begin
                minValue := a[i];
                minIndex := j;
            end
            j := j + 1;
        end
        
        { Swap the min element into position i }
        temp := a[i];
        a[i] := minValue;
        a[minIndex] := temp;
        
        i := i + 1;
    end
end

begin
    { This is where we'd assign some values in a... }
    
    selectionSort(a);
    
    print(a);

end. 