import React from 'react';
import { graphql } from 'react-apollo';
import gql from 'graphql-tag';
import { compose, pure, withState, withHandlers } from 'recompose';

const enhance = compose(
    graphql(gql`
            mutation createGame($name: String!) {
                createGame(name: $name) {
                    uuid
                    jsonState
                }
            }
        `, {
        name: 'createGame',
        props: ({ createGame, ownProps: { setGameUuid } }) => ({
            createGame: async (name) => {
                const data = await createGame({ variables: {name} })
                setGameUuid(data.data.createGame.uuid);
            }
        }),
    }),
    withState('value', 'updateValue', ''),
    withHandlers({
      onChange: props => event => props.updateValue(event.target.value),
    }),
    pure
);

export const CreateGame = enhance(({ createGame, errors, value, onChange }) => {
    return (
      <div>
        <input type="name" onChange={onChange} value={value} />
        <button onClick={() => createGame(value)}>Create Game</button>
        <ul>{errors && errors.map(e => <li>{e}</li>)}</ul>
      </div>
    );
});


export default CreateGame;
