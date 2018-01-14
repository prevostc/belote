import React from 'react';
import './App.css';
import { graphql } from 'react-apollo';
import gql from 'graphql-tag';
import { compose, pure, withState, withHandlers } from 'recompose';

const enhance = compose(
    withState('value', 'updateValue', ''),
    withHandlers({
      onChange: props => event => {
        props.updateValue(event.target.value)
      },
    }),
    pure
);

export const App = enhance(({ createGame, errors, value, onChange }) => {
    return (
      <div>
        <input type="name" onChange={onChange} value={value} />
        <button onClick={() => createGame(value)}>Create Game</button>
        <ul>{errors && errors.map(e => <li>{e}</li>)}</ul>
      </div>
    );
});

export const GameFragment = gql`
    fragment GameFragment on Game {
        uuid
        jsonState
    }
`;

export const AppWithMutations =
  graphql(gql`
        mutation createGame($name: String!) {
            createGame(name: $name) {
                ...GameFragment
            }
        }

        ${GameFragment}
    `, {
  name: 'createGame',
  alias: 'createGame',
  options: {
    errorPolicy: 'all'
  },
  props: ({ createGame }) => ({
    createGame: (name) => {
      createGame({
        variables: { name },
      })
    },
  }),
})(App);


export default AppWithMutations;
