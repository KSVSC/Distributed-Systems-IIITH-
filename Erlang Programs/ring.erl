-module(ring).
-export([start/4, init_process/3,main/1]).

main([I,O]) ->
  {ok, File1} = file:open(I,[read]),
  [P, X] = string:tokens(io:get_line(File1,'')," "),
  [T] = string:tokens(X,"\n"), 
  {ok, File2} = file:open(O, [write]),
  start(list_to_integer(P),list_to_integer(P),list_to_integer(T), File2).


start(M, N, Message, File2) ->
  { ok, NextPid } = create_process(N, M, File2),
  ok = send_to_ring(Message, N, NextPid, File2),
  ok.

create_process(N, M, File2) ->
  create_process(N, M, self(), File2).
      
create_process(1, M, NextOnTheRing, File2)
  when M > 0 ->
  { ok, NextOnTheRing };
            
create_process(N, M, NextOnTheRing, File2) ->
  ID = N-1,
  Pid = spawn(ring, init_process, [NextOnTheRing, ID, File2]),
  create_process(N - 1, M, Pid, File2).
  
init_process(NextOnTheRing, ID, File2) ->
  loop(NextOnTheRing, ID, File2).

loop(NextOnTheRing, ID, File2) ->
  receive
    { Msg, From} ->
      % io:format("Process ~w received token ~w from process ~w\n", [ID, Msg, ID-1]),
      io:format(File2, "Process ~w received token ~w from process ~w\n", [ID, Msg, ID-1]),
      NextOnTheRing ! { Msg, self() },
      loop(NextOnTheRing, ID, File2)
  end.

send_to_ring(Msg, N, NextPid, File2) ->
  NextPid ! { Msg, self() },
  ID = N-1,
  ok = receive_from_last_node(Msg, ID, File2).
  
receive_from_last_node(Msg, ID, File2) ->
  receive
      { Msg, From } ->
      % io:format("Process ~w received token ~w from process ~w\n", [0, Msg, ID]),
      io:format(File2, "Process ~w received token ~w from process ~w\n", [0, Msg, ID]),
      ok
  end.