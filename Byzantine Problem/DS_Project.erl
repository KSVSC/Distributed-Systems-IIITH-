-module('DS_Project').
-import(lists,[nth/2, map/2, seq/2, append/2]).
-import(string,[tokens/2]).
-export([start/1]).

read_file(File) ->
    case file:read_line(File) of
        {ok, Data} ->
            [First|Rest] = string:tokens(Data,"\t\n"),
            [{First,string:join(Rest,"\t")} | read_file(File)];
        eof -> []
    end.

each_process(Index, Alpha, Weights, PIDS, Value) ->
    map(
        fun(T)->
            Queen_ID = T,
            Sum0 = 0,
            Sum1 = 0,
            KWValue = nth(Index,Weights),

            if KWValue > 0 ->
                    map(
                        fun(K) ->
                            if K /= Index ->
                                    nth(K,PIDS) ! {decided_value, Value, Index},
                                    io:fwrite("Sending ~p to ~p from ~p ~n", [Value,K,Index]);
                                true ->
                                    K
                            end
                        end,
                    seq(1,length(PIDS)));
                true ->
                    T
            end,

            Sum = map(
                fun(K) ->
                    Temp_KWValue = nth(K,Weights),
                    if Temp_KWValue > 0 andalso K /= Index->
                            receive
                                {decided_value, ValueTemp, K} ->
                                    io:fwrite("Process ~p received Value ~p from ~p~n",[Index, ValueTemp, K]),
                                    if Value == 1 ->
                                            {0, nth(K,Weights)};
                                        true ->
                                            {nth(K,Weights), 0}
                                    end
                            end;
                        true ->
                            {Sum0, Sum1}
                    end
                end,
            seq(1,length(PIDS))),
            io:fwrite("Process ~p ~p~n",[Index, Sum])
        end,
    seq(1,Alpha)).

start(Token) ->
    {ok, FileRead} = file:open(nth(1,Token),[read]),
%    {ok, FileWrite} = file:open(nth(2,Token), [write]),
    
    Data = read_file(FileRead),
    {Num,[]}=nth(1,Data),
    Num_Proc = nth(1,tokens(Num," ")),
    {Value,[]}=nth(2,Data),
    Values = tokens(Value," "),
    {Weight,[]}=nth(3,Data),
    Weights = tokens(Weight," "),
    {Faulty,[]}=nth(4,Data),
    FaultyProc = tokens(Faulty," "),
    Alpha = 1,

    Rest_PIDS = map(
        fun(T) -> 
            Pid = spawn(fun() -> 
                        receive
                            {PIDS} ->
                                 each_process(T, Alpha, Weights, PIDS, nth(T,Values))
                                %  io:fwrite("Get over ~p~n",[self()])                            
                        end
                    end),
            Pid
        end,
        seq(2,list_to_integer(Num_Proc))),

    PIDS = append([self()], Rest_PIDS),
    map(
        fun(T) ->
            nth(T,PIDS) ! {PIDS}
        end,
    seq(2,list_to_integer(Num_Proc))),    
each_process(1, Alpha, Weights, PIDS, nth(1,Values)).
% io:fwrite("Get over~p~n",[self()]).