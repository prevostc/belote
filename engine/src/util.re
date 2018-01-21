module Util {
    include Random;

    let intCompare = (a: int, b: int) => a < b ? 1 : (a > b ? -1 : 0); 

    let listLast = lst => List.length(lst) === 0 ? None : Some(List.length(lst) -1 |> List.nth(lst));

    let listProduct = (l1, l2) => 
        List.map(e1 => List.map(e2 => (e1,e2), l2), l1)
        |> List.concat;

    let listShuffle = lst => lst |> List.sort((_, _) => Random.int(3) - 1);

    let listEq = (cmp, lstA, lstB) => List.combine(lstA, lstB) 
        |> List.fold_left((agg, e) => switch e {
            | (a, b) => agg && cmp(a, b)
        }, true);

    let listSlice = (start, stop, lst) => lst 
        |> List.mapi((i, e) => (i, e)) 
        |> List.filter(((i, _)) => i >= start && i <= stop)
        |> List.map(((_, e)) => e);

    let listFind = filter => List.fold_left((agg, e) => switch agg {
        | None => filter(e) ? Some(e) : None
        | Some(a) => Some(a)
    }, None);

    let arrayFind = filter => Array.fold_left((agg, e) => switch agg {
        | None => filter(e) ? Some(e) : None
        | Some(a) => Some(a)
    }, None);

    let arrayFilter = filter => Array.fold_left((agg, e) => filter(e) ? Array.append(agg, [| e |]) : agg, [| |]);
}