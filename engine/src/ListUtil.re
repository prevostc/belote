/* @todo: use bucklescript Belt library ? */

let last = lst => List.length(lst) === 0 ? None : Some(List.length(lst) -1 |> List.nth(lst));

let product = (l1, l2) =>
    List.map(e1 => List.map(e2 => (e1,e2), l2), l1)
    |> List.concat;

let shuffle = lst => lst
    |> List.map(e => (e, Random.int(1000) - 1))
    |> List.sort(((_, a), (_, b)) => compare(a, b))
    |> List.map((t) => fst(t));

let equals = (cmp, lstA, lstB) => List.combine(lstA, lstB)
    |> List.fold_left((agg, e) => switch e {
        | (a, b) => agg && cmp(a, b)
    }, true);

let slice = (start, stop, lst) => lst
    |> List.mapi((i, e) => (i, e))
    |> List.filter(((i, _)) => i >= start && i <= stop)
    |> List.map(((_, e)) => e);

let find = filter => List.fold_left((agg, e) => switch agg {
    | None => filter(e) ? Some(e) : None
    | Some(a) => Some(a)
}, None);

let indexOf = (e, eq, lst): option(int) => {
    let filteredList = lst
       |> List.mapi((i, el) => (i, el))
       |> List.filter(((_, el)) => eq(el, e));
    if (List.length(filteredList) > 0) {
        let (i, _) = filteredList |> List.hd;
        Some(i)
    } else {
        None
    }
};

let flatMap = (f, lst) => lst |> List.fold_left((agg, e) => agg @ f(e), []);
