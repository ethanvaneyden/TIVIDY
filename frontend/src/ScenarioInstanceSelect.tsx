import { Container, Row, Col } from 'react-bootstrap';
import { useNavigate, useParams } from 'react-router';
import './ScenarioInstanceSelect.css';

const instanceIds = ['INS001', 'INS002'];

function ScenarioInstanceSelect() {
    const { scenario: scenarioId } = useParams();
    const navigate = useNavigate();

    return (
        <Container className="py-5">
            <div className="mb-5">
                <p className="text-primary fw-semibold text-uppercase small mb-2">
                    {scenarioId}
                </p>

                <h1 className="landing-header mb-3">
                    Choose an instance
                </h1>

                <p className="text-secondary mb-0">
                    Select an instance to continue.
                </p>
            </div>

            <Row className="g-3">
                {instanceIds.map((instanceId) => (
                    <Col key={instanceId} xs={12} md={6} lg={4}>
                        <div className="scenario-instance-wrapper">
                            <button
                                type="button"
                                className="scenario-instance-card w-100 d-flex align-items-center gap-3 p-4 text-start"
                                onClick={() => navigate(`/${scenarioId}/${instanceId}`)}
                            >
                                <h2 className="h5 mb-1">{instanceId}</h2>
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

            <Row className="mt-4">
                <Col>
                    <button type="button" className="cta-button">
                        + Create Instance
                    </button>
                </Col>
            </Row>
        </Container>
    );
}

export default ScenarioInstanceSelect;
