-module(merge).
-export([main/1, parallel_merge/2, merge_sort/2]).

main([I,O]) ->
	{ok, File1} = file:open(I,[read]),
	{ok,A} = file:read(File1,1024 * 1024),
	Inp = lists:map(fun(X) -> {Int, _} = string:to_integer(X), 
					Int end, 
		  string:tokens(A, " ")),
	parallel_merge(Inp, O).

input(Pid)->
	receive
		{Pid, I} -> I
	end.

parallel_merge(I, File2) ->
	Pid = spawn(?MODULE, merge_sort, [self(), I]),
	L = input(Pid),
	List = lists:map(fun(X) -> integer_to_list(X) end, L),
	Out = string:join(List," "),	
	file:write_file(File2, Out).

merge(L1, []) -> L1;
merge([], L2) -> L2;
merge([A1|A2], [B1|B2]) ->
	if
		A1 < B1 -> [A1 | merge(A2, [B1|B2])];
		true -> [B1 | merge([A1|A2], B2)]
	end.

merge_sort(Pid, I) when length(I) < 2 -> Pid! {self(), I};
merge_sort(Pid, I) when length(I) > 1 ->
	{A1, A2} = lists:split(length(I) div 2, I),
	P1 = spawn(?MODULE, merge_sort, [self(), A1]),
	P2 = spawn(?MODULE, merge_sort, [self(), A2]),
	B1 = input(P1),
	B2 = input(P2),
	Pid ! {self(), merge(B1, B2)}.