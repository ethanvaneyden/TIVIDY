interface HistoryPanelProps {
    entries: string[];
}

function HistoryPanel({ entries }: HistoryPanelProps) {
    return (
        <section className="history-panel" aria-label="Workflow history">
            <div className="history-panel-header">
                <span className="workflow-section-label">HISTORY</span>
                <span className="history-count">{entries.length} events</span>
            </div>
            {entries.length > 0 ? (
                <ol className="history-list">
                    {entries.map((entry, index) => (
                        <li key={`${index}-${entry}`}>{entry}</li>
                    ))}
                </ol>
            ) : (
                <p className="history-empty">Nothing has happened in this workflow yet.</p>
            )}
        </section>
    );
}

export default HistoryPanel;
