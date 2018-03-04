import React from 'react';
import Board from './game/Board';
import Menu from './menu';
import { compose, pure } from 'recompose';
import {
    BrowserRouter as Router,
    Route
} from 'react-router-dom';

const uuid = require('uuid/v4');

const playerUuid = uuid();

const enhance = compose(
    pure
);

export default enhance(() => {
  return <Router>
      <div id="app">
          <Route exact path="/" render={() => <Menu playerUuid={playerUuid} />} />
          <Route path="/game/:uuid" render={({ match }) => <Board playerUuid={playerUuid} gameUuid={match.params.uuid} /> } />
      </div>
  </Router>
});