let find = filter => Array.fold_left((agg, e) => switch agg {
    | None => filter(e) ? Some(e) : None
    | Some(a) => Some(a)
}, None);

let filter = filter => Array.fold_left((agg, e) => filter(e) ? Array.append(agg, [| e |]) : agg, [| |]);