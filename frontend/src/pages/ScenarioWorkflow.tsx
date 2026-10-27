import { Container } from 'react-bootstrap';
import { useNavigate, useParams } from 'react-router';
import EducationWorkflow from '../features/education/EducationWorkflow';
import HealthcareWorkflow from '../features/healthcare/HealthcareWorkflow';
import type { ScenarioId } from '../types/types';
import '../styles/Scenario.css';

function ScenarioWorkflow() {
    const { scenario: scenarioParam, instanceId } = useParams();
    const navigate = useNavigate();
    const scenario = isScenarioId(scenarioParam) ? scenarioParam : undefined;

    if (!scenario || !instanceId) {
        return (
            <Container className="scenario-page py-5">
                <button type="button" className="chooser-back" onClick={() => navigate('/')}>
                    <span aria-hidden="true">←</span>
                    All workspaces
                </button>
                <h1 className="landing-header">Workflow not found</h1>
                <p className="scenario-description">Choose a workspace and instance to continue.</p>
            </Container>
        );
    }

    const scenarioName = scenario === 'education' ? 'Assignment marking' : 'Patient admissions';

    return (
        <Container className="scenario-page py-5" data-scenario={scenario}>
            <button
                type="button"
                className="chooser-back scenario-workflow-back"
                onClick={() => navigate(`/${scenario}/instances`)}
            >
                <span aria-hidden="true">←</span>
                Choose {scenarioName.toLowerCase()} instance
            </button>

            <header className="scenario-workflow-header">
                <span className="scenario-eyebrow">{scenarioName}</span>
                <h1 className="landing-header">
                    {scenario === 'education' ? 'Marking workspace' : 'Admissions workspace'}
                </h1>
                <p className="scenario-description">
                    Instance <span className="instance-text">{instanceId}</span>
                </p>
            </header>

            {scenario === 'education'
                ? <EducationWorkflow key={instanceId} instanceId={instanceId} />
                : <HealthcareWorkflow key={instanceId} instanceId={instanceId} />}
        </Container>
    );
}

function isScenarioId(value: string | undefined): value is ScenarioId {
    return value === 'education' || value === 'healthcare';
}

export default ScenarioWorkflow;
