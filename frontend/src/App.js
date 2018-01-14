import React, { Component } from 'react';
import CreateGame from './menu/CreateGame';
import Board from './game/Board';
import { compose, withState } from 'recompose';

const enhance = compose(
    withState('uuid', 'setGameUuid', ''),
);

export default enhance(({ uuid, setGameUuid }) => {
  return uuid
      ? <Board uuid={uuid} />
      : <CreateGame setGameUuid={setGameUuid} />
});