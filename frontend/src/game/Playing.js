import React from 'react';
import { graphql } from "react-apollo/index";
import { branch, compose, lifecycle, pure, renderComponent } from "recompose";
import Card from './Card';
import { CardsQuery, playCardMutation, playCardMutationUpdate, subscribeToChange } from "../api/card";

const enhance = compose(
    graphql(CardsQuery, {
        props: ({ data }) => {
            const { loading, error, game } = data;
            return ({
                loading,
                error,
                cards: game ? game.cards : [],
                subscribeToChange: subscribeToChange(data),
            })
        },
        options: ({ gameUuid, playerUuid }) => ({
            variables: { gameUuid, playerUuid },
        }),
    }),
    graphql(playCardMutation, {
        name: 'playCard',
        props: ({ playCard, ownProps: { gameUuid, playerUuid } }) => ({
            playCard: (color, motif) => {
                const variables = { gameUuid, playerUuid, color, motif };
                return playCard({
                    variables,
                    update: playCardMutationUpdate({ gameUuid, playerUuid }),
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
    pure
);


export const Playing = enhance(({ cards, playCard }) => {
    return (
      <div>
          <h1>Card</h1>
          <ol>
            {cards.map(({color, motif}) => <li key={`${color}-${motif}`}>
                <Card color={color} motif={motif} />
                <button onClick={() => playCard(color, motif)}>Play this</button>
            </li>)}
          </ol>
      </div>
    );
});

export default Playing;
