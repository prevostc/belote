import React from 'react';
import { graphql } from 'react-apollo';
import gql from 'graphql-tag';
import { compose, pure, branch, renderComponent } from 'recompose';
import JoinGame from './JoinGame';

const enhance = compose(
    graphql(gql`
            query game($uuid: String!) {
                game(uuid: $uuid) {
                    uuid
                    gameState
                    players {
                        uuid
                        spot
                        name
                    }
                }
            }
        `, {
        props: ({ data: { loading, error, game } }) => ({
            loading,
            error,
            game,
        }),
        options: ({ uuid }) => ({
            variables: { uuid },
        }),

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
          {players.map(({name, spot}) => <div>{name} - {spot}</div>)}
          <JoinGame gameUuid={uuid} />
      </div>
    );
});

export default Board;
