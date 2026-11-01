import { useState } from 'react';
import { Alert, Container, Row, Col } from 'react-bootstrap';
import { useNavigate, useParams } from 'react-router';
import '../styles/ScenarioInstanceSelect.css';
import '../styles/Scenario.css';
import CreateInstanceModal from '../components/modals/CreateInstanceModal';
import DeleteInstanceModal from '../components/modals/DeleteInstanceModal';
import { LoadFailure, Loading } from '../components/shared/LoadState';
import { INSTANCE_DELETE_ENABLED } from '../api/workflow';
import { useInstances } from '../hooks/useInstances';
import type { ScenarioId, ScenarioInstance } from '../types/types';
import '../styles/Shared.css';

function ScenarioInstanceSelect() {
    const { scenario: scenarioId } = useParams();
    const navigate = useNavigate();
    const scenario = isScenarioId(scenarioId) ? scenarioId : undefined;

    if (!scenario) {
        return (
            <Container className="scenario-page py-5">
                <button type="button" className="chooser-back" onClick={() => navigate('/')}>
                    <span aria-hidden="true">←</span>
                    All workspaces
                </button>
                <h1 className="landing-header">Workspace not found</h1>
                <p className="scenario-description">Choose a workspace from the home page.</p>
            </Container>
        );
    }

    return <ScenarioChooser key={scenario} scenario={scenario} />;
}

function ScenarioChooser({ scenario }: { scenario: ScenarioId }) {
    const navigate = useNavigate();
    const scenarioName = scenario === 'education' ? 'Assignment marking' : 'Patient admissions';
    const hub = useInstances(scenario);
    const [showCreateModal, setShowCreateModal] = useState(false);
    const [showDeleteModal, setShowDeleteModal] = useState(false);
    const [selectedInstance, setSelectedInstance] = useState<ScenarioInstance>();

    return (
        <Container className="scenario-page py-5" data-scenario={scenario}>
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
                            Select an instance to view its containers and items.
                        </p>
                    </div>
                </div>
            </header>

            <div className="chooser-list-heading">
                <h2>Your instances</h2>
                <span>Select one to continue</span>
            </div>

            {hub.actionError && (
                <Alert variant="danger" className="chooser-error" dismissible onClose={hub.clearActionError}>
                    {hub.actionError}
                </Alert>
            )}

            {hub.loading && hub.instances.length === 0 ? (
                <Loading label="Loading instances..." />
            ) : hub.loadError && hub.instances.length === 0 ? (
                <LoadFailure
                    title="Could not load instances"
                    message={hub.loadError}
                    onRetry={() => void hub.refresh()}
                />
            ) : hub.instances.length > 0 ? (
            <Row className="g-3 chooser-grid">
                {hub.instances.map((instance, index) => (
                    <Col key={instance.id} xs={12} md={6}>
                        <div className="scenario-instance-wrapper">
                            <button
                                type="button"
                                className="scenario-instance-card w-100 d-flex align-items-center gap-3 p-4 text-start"
                                onClick={() => navigate(`/${scenario}/${instance.id}`)}
                            >
                                <span className="instance-number" aria-hidden="true">
                                    {String(index + 1).padStart(2, '0')}
                                </span>
                                <span className="instance-copy">
                                    <span className="instance-label">{instance.id}</span>
                                    <span className="instance-name">{instance.name}</span>
                                </span>
                                <span className="instance-open" aria-hidden="true">↗</span>
                            </button>
                            {INSTANCE_DELETE_ENABLED && (
                                <button
                                    type="button"
                                    className="scenario-instance-delete"
                                    aria-label={`Delete instance ${instance.name}`}
                                    onClick={() => {
                                        setSelectedInstance(instance);
                                        setShowDeleteModal(true);
                                    }}
                                >
                                    ×
                                </button>
                            )}
                        </div>
                    </Col>
                ))}
            </Row>
            ) : (
                <div className="chooser-empty">
                    <span className="chooser-empty-icon" aria-hidden="true">＋</span>
                    <h2>No instances yet</h2>
                    <p>Create an instance to start a workflow.</p>
                </div>
            )}

            <Row className="mt-4 chooser-create-row">
                <Col>
                    <button
                        type="button"
                        className="cta-button chooser-create"
                        onClick={() => setShowCreateModal(true)}
                        disabled={hub.busy}
                    >
                        <span aria-hidden="true">+</span>
                        Create new instance
                    </button>
                </Col>
            </Row>

            <CreateInstanceModal
                show={showCreateModal}
                busy={hub.busy}
                onHide={() => setShowCreateModal(false)}
                onSubmit={async (name) => {
                    const id = await hub.create(name);
                    if (id === undefined) return false;
                    navigate(`/${scenario}/${id}`);
                    return true;
                }}
            />
            <DeleteInstanceModal
                show={showDeleteModal}
                instanceName={selectedInstance?.name}
                busy={hub.busy}
                onHide={() => {
                    setShowDeleteModal(false);
                    setSelectedInstance(undefined);
                }}
                onConfirm={() => hub.remove(selectedInstance?.id ?? '')}
            />
        </Container>
    );
}

function isScenarioId(value: string | undefined): value is ScenarioId {
    return value === 'education' || value === 'healthcare';
}

export default ScenarioInstanceSelect;
