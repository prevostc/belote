import React from 'react';
import { graphql } from 'react-apollo';
import gql from 'graphql-tag';
import { compose, pure, branch, renderComponent } from 'recompose';

const enhance = compose(
    graphql(gql`
            query game($uuid: String!) {
                game(uuid: $uuid) {
                    uuid
                    jsonState
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
    pure
);

export const Board = enhance(({ game: { uuid, jsonState }}) => {
    return (
      <div>
          <div>{uuid}</div>
          <div>{jsonState}</div>
      </div>
    );
});

console.log(Board)


export default Board;
