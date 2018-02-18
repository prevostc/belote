import React from 'react';
import {graphql} from "react-apollo/index";
import {branch, compose, lifecycle, pure, renderComponent, withHandlers, withState} from "recompose";
import {BidsQuery, bidMutation, bidMutationUpdate, subscribeToChange} from "../api/bid";

// @todo: get those from the engine lib
const colors = ['SPADES', 'HEARTS', 'DIAMONDS', 'CLUBS'];

const enhance = compose(
    graphql(BidsQuery, {
        props: ({ data }) => {
            console.log("BidsQuery",data)
            const { loading, error, game } = data;
            return ({
                loading,
                error,
                game,
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
            bid: async (value, color) => {
                const variables = { gameUuid, playerUuid, value, color };
                console.log("bidMutation: variables",variables)
                const data = await bid({
                    variables,
                    update: bidMutationUpdate({ gameUuid, playerUuid }),
                });
                console.log("bidMutation: data",data)
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


export const BidPhase = enhance((props) => {
    console.log(props)
    const {value, onValueChange, color, onColorChange, bid, pass} = props;
    return (
      <div>
          <h1>Bidding</h1>
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

export default BidPhase;
