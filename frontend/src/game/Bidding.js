import React from 'react';
import {graphql} from "react-apollo/index";
import {branch, compose, lifecycle, pure, renderComponent, withHandlers, withState} from "recompose";
import {BidsQuery, bidMutation, bidMutationUpdate, subscribeToChange, passMutation, passMutationUpdate} from "../api/bid";

// @todo: get those from the engine lib
const colors = ['SPADES', 'HEARTS', 'DIAMONDS', 'CLUBS'];

const enhance = compose(
    graphql(BidsQuery, {
        props: ({ data }) => {
            const { loading, error, game } = data;
            return ({
                loading,
                error,
                bids: game ? game.bids : [],
                subscribeToChange: subscribeToChange(data),
            })
        },
        options: ({ gameUuid, playerUuid }) => ({
            variables: { gameUuid, playerUuid },
        }),
    }),
    graphql(bidMutation, {
        name: 'bid',
        props: ({ bid, ownProps: { gameUuid, playerUuid } }) => ({
            bid: (value, color) => {
                const variables = { gameUuid, playerUuid, value, color };
                return bid({
                    variables,
                    update: bidMutationUpdate({ gameUuid, playerUuid }),
                });
            }
        }),
    }),
    graphql(passMutation, {
        name: 'pass',
        props: ({ pass, ownProps: { gameUuid, playerUuid } }) => ({
            pass: () => {
                const variables = { gameUuid, playerUuid };
                return pass({
                    variables,
                    update: passMutationUpdate({ gameUuid, playerUuid }),
                });
            }
        }),
    }),
    lifecycle({
        componentWillMount() {
            this.props.subscribeToChange({
                uuid: this.props.gameUuid,
            });
        }
    }),
    branch(
        props => props.loading,
        renderComponent(() => <div>LOADING</div>),
    ),
    branch(
        props => props.error,
        renderComponent(({ error }) => <div>ERROR: {JSON.stringify(error)}</div>),
    ),
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
