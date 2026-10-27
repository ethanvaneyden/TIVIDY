import type { Submission } from "./types";
import { Row, Col } from "react-bootstrap";
import "./SubmissionRow.css";

function SubmissionRow({
  name,
  state,
  assignee,
  reviewer,
  reviewMethod,
  onAction
}: Submission & { onAction: (action: string) => void }) {

  const renderButtons = () => {
    switch (state) {
      case "available":
        return <button className="cta-button">Assign TA</button>;
      case "assigned":
        return <button className="cta-button" onClick={() => onAction("start")}>Start</button>;
      case "inprogress":
        return (
          <>
            <button className="cta-button success" onClick={() => onAction("complete")}>Mark</button>
          </>
        );
      case "completed":
        return <button className="cta-button outline">View</button>;
      case "rejected":
        return <button className="cta-button warning" onClick={() => onAction("override")}>Override</button>;
      default:
        return null;
    }
  };

  return (
    <Row className={`item-row state-${state} align-items-center g-0`}>
      
      <Col xs={12} md={4} className="row-section mb-3 mb-md-0">
        <strong className="item-name">{name}</strong>
        <span className="item-state">{state}</span>
      </Col>

      <Col xs={12} md={5} className="row-section mb-3 mb-md-0">
        <span className="context-text">
          <strong>TA:</strong> {assignee || "Unassigned"}
        </span>
        {reviewer && (
          <span className="context-text">
            <strong>Reviewer:</strong> {reviewer} ({reviewMethod})
          </span>
        )}
      </Col>

      <Col xs={12} md={3} className="d-flex justify-content-md-end gap-2">
        {renderButtons()}
      </Col>

    </Row>
  );
}

export default SubmissionRow;