import React from 'react';
import {compose, pure, withHandlers, withState} from "recompose";
import { connectBidsMutations } from "../api";

// @todo: get those from the engine lib
const colors = ['SPADES', 'HEARTS', 'DIAMONDS', 'CLUBS'];

const enhance = compose(
    connectBidsMutations,
    withState('value', 'updateValue', 80),
    withHandlers({
        onValueChange: props => event => props.updateValue(event.target.value),
    }),
    withState('color', 'updateColor', colors[0]),
    withHandlers({
        onColorChange: props => event => props.updateColor(event.target.value),
    }),
    pure
);


export const Bidding = enhance(({value, onValueChange, color, onColorChange, bid, pass, bids}) => {
    return (
      <div>
          <h1>Bidding</h1>
          <h2>Bids</h2>
          <ol>
            {bids.map((b, i) => b.isPass
                ? <li key={i}>{b.player.spot}: Pass</li>
                : <li key={i}>{b.player.spot}: Bid {b.value} {b.color}</li>
            )}
          </ol>
          <h2>New Bid</h2>
          <label>
              <span>Value :</span>
              <input name="value" placeholder="value" type="number" onChange={onValueChange} value={value} />
          </label>
          <label>
              <span>Color :</span>

              <select value={color} onChange={onColorChange}>
                  {colors.map(c => <option key={c} value={c}>{c}</option>)}
              </select>
          </label>
          <button onClick={() => bid(value, color)}>Bid {value} {color}</button>
          <button onClick={() => pass()}>Pass</button>
      </div>
    );
});

export default Bidding;
