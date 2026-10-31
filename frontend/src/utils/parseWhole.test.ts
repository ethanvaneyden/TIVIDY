import { describe, expect, it } from 'vitest';
import { parseWhole } from './parseWhole';

describe('parseWhole', () => {
    it('accepts whole numbers inside the range, including both ends', () => {
        expect(parseWhole('0', 0, 10)).toBe(0);
        expect(parseWhole('10', 0, 10)).toBe(10);
        expect(parseWhole(' 7 ', 0, 10)).toBe(7);
        expect(parseWhole('100', 0, 100)).toBe(100);
    });

    it('rejects out-of-range, decimal, signed and non-numeric input', () => {
        expect(parseWhole('11', 0, 10)).toBeUndefined();
        expect(parseWhole('0', 1, 10)).toBeUndefined();
        expect(parseWhole('2.5', 0, 10)).toBeUndefined();
        expect(parseWhole('-1', 0, 10)).toBeUndefined();
        expect(parseWhole('abc', 0, 10)).toBeUndefined();
        expect(parseWhole('', 0, 10)).toBeUndefined();
    });
});
