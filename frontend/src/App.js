import React from 'react';
import CreateGame from './menu/CreateGame';
import JoinGame from './menu/JoinGame';
import Board from './game/Board';
import { compose, withState } from 'recompose';

const enhance = compose(
    withState('uuid', 'setGameUuid', ''),
);

export default enhance(({ uuid, setGameUuid }) => {
  return uuid
      ? <Board uuid={uuid} />
      : <div>
          <CreateGame setGameUuid={setGameUuid} />
          <JoinGame setGameUuid={setGameUuid} />
      </div>
});