import { Routes, Route } from 'react-router';

import Landing from './Landing';
import ScenarioInstanceSelect from './ScenarioInstanceSelect';
import ScenarioWorkflow from './ScenarioWorkflow';

function App() {
    return (
        <Routes>
            <Route
                path="/"
                element={<Landing />}
            />

            <Route
                path="/:scenario/instances"
                element={<ScenarioInstanceSelect />}
            />
            <Route
                path="/:scenario/:instanceId"
                element={<ScenarioWorkflow />}
            />
        </Routes>
    );
}

export default App;