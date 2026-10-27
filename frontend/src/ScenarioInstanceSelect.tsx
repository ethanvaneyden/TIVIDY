import { Container, Row, Col } from 'react-bootstrap';
import { useNavigate, useParams } from 'react-router';
import './ScenarioInstanceSelect.css';
import './Scenario.css';

const instanceIds = ['INS001', 'INS002'];

function ScenarioInstanceSelect() {
    const { scenario: scenarioId } = useParams();
    const navigate = useNavigate();
    const scenarioName = scenarioId
        ?.replace(/[-_]/g, ' ')
        .replace(/\b\w/g, (letter) => letter.toUpperCase()) ?? 'Scenario';

    return (
        <Container className="scenario-page py-5" data-scenario={scenarioId?.toLowerCase()}>
            <button
                type="button"
                className="chooser-back"
                onClick={() => navigate('/')}
            >
                <span aria-hidden="true">←</span>
                All workspaces
            </button>

            <header className="chooser-header">
                <span className="scenario-eyebrow">{scenarioName}</span>
                <div className="chooser-title-row">
                    <div>
                        <p className="chooser-kicker">{scenarioName} workspace</p>
                        <h1 className="landing-header mb-3">Choose an instance</h1>
                        <p className="scenario-description mb-0">
                            Pick up where you left off, or start a new instance.
                        </p>
                    </div>
                    <span className="chooser-count">
                        <strong>{instanceIds.length.toString().padStart(2, '0')}</strong>
                        <span>instances</span>
                    </span>
                </div>
            </header>

            <div className="chooser-list-heading">
                <h2>Your instances</h2>
                <span>Select one to continue</span>
            </div>

            <Row className="g-3 chooser-grid">
                {instanceIds.map((instanceId, index) => (
                    <Col key={instanceId} xs={12} md={6}>
                        <div className="scenario-instance-wrapper">
                            <button
                                type="button"
                                className="scenario-instance-card w-100 d-flex align-items-center gap-3 p-4 text-start"
                                onClick={() => navigate(`/${scenarioId}/${instanceId}`)}
                            >
                                <span className="instance-number" aria-hidden="true">
                                    {String(index + 1).padStart(2, '0')}
                                </span>
                                <span className="instance-copy">
                                    <span className="instance-label">Instance</span>
                                    <span className="instance-name">{instanceId}</span>
                                </span>
                                <span className="instance-open" aria-hidden="true">↗</span>
                            </button>

                            <button
                                type="button"
                                className="scenario-instance-delete"
                                aria-label={`Delete instance ${instanceId}`}
                            >
                                ×
                            </button>
                        </div>
                    </Col>
                ))}
            </Row>

            <Row className="mt-4 chooser-create-row">
                <Col>
                    <button type="button" className="cta-button chooser-create">
                        <span aria-hidden="true">+</span>
                        Create new instance
                    </button>
                    <p className="chooser-create-hint">
                        Start a fresh {scenarioName.toLowerCase()} workflow.
                    </p>
                </Col>
            </Row>
        </Container>
    );
}

export default ScenarioInstanceSelect;
