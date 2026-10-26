import { Routes, Route } from 'react-router';

import Landing from './Landing';
import EducationInstanceSelect from './EducationInstanceSelect';

function App() {
    return (
        <Routes>
            <Route
                path="/"
                element={<Landing />}
            />

            <Route
                path="/education/instances"
                element={<EducationInstanceSelect ids={["INS001", "INS002"]} />}
            />
        </Routes>
    );
}

export default App;