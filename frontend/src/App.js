import React from 'react';
import Board from './game/Board';
import Menu from './menu';
import { compose, pure } from 'recompose';
import {
    BrowserRouter as Router,
    Route,
    Link
} from 'react-router-dom'

const enhance = compose(
    pure
);

export default enhance(() => {
  return <Router>
      <div>
          <Route exact path="/" render={() => <Menu />} />
          <Route path="/game/:uuid" render={({ match }) => <Board uuid={match.params.uuid} /> } />
      </div>
  </Router>
});