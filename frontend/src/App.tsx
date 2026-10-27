import { Routes, Route } from 'react-router';

import Landing from './Landing';
import EducationInstanceSelect from './EducationInstanceSelect';
import EducationWorkflow from './EducationWorkflow';

function App() {
    return (
        <Routes>
            <Route
                path="/"
                element={<Landing />}
            />

            <Route
                path="/education/instances"
                element={<EducationInstanceSelect/>}
            />
            <Route
                path="education/:instanceid"
                element={<EducationWorkflow />}
            />
        </Routes>
    );
}

export default App;