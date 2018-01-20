import React from 'react';
import { graphql } from 'react-apollo';
import { compose, pure, branch, renderComponent, lifecycle } from 'recompose';
import JoinGame from './JoinGame';
import { gameQuery, subscribeToChange } from "../api/game";


const enhance = compose(
    graphql(gameQuery, {
        props: ({ data }) => {
            const { loading, error, game } = data;
            return ({
                loading,
                error,
                game,
                subscribeToChange: subscribeToChange(data),
            })
        },
        options: ({ uuid }) => ({
            variables: { uuid },
        }),
    }),
    lifecycle({
        componentWillMount() {
            this.props.subscribeToChange({
                uuid: this.props.uuid,
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

export const Board = enhance(({ game: { uuid, gameState, players }}) => {
    return (
      <div>
          <div>{uuid}</div>
          <div>{gameState}</div>
          {players.map(({name, uuid, spot}) => <div key={spot}>{uuid} - {name} - {spot}</div>)}
          <JoinGame gameUuid={uuid} />
      </div>
    );
});

export default Board;
