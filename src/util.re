module Util {

    let listLast = lst => List.length(lst) === 0 ? None : Some(List.length(lst) -1 |> List.nth(lst));

    let listProduct = (l1, l2) => 
        List.map(e1 => List.map(e2 => (e1,e2), l2), l1)
        |> List.concat
    ;
}