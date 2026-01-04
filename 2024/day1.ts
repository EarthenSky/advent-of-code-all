
function minAndIndex(list:number[]): [number, number] | null {
    if (list.length === 0)
        return null;
    
    var min = list[0];
    var min_i = 0;
    for (var i = 1; i < list.length; i++) {
        if (list[i] < min) {
            min = list[i];
            min_i = i;
        }
    }

    return [min, min_i];
}

function bothParts(contents: string): number | null {
    const pairs: [number, number][] =
        contents
        .trim()
        .split("\n")
        .map(str => str.split("   "))
        .map(str => [
            Number(str[0]),
            Number(str[1])
        ]);
  
    const leftOccuranceMap = new Map();
    const rightOccuranceMap = new Map();
    const left: number[] = [];
    const right: number[] = [];
    for (const [l, r] of pairs) {
        left.push(l);
        right.push(r);

        if (leftOccuranceMap.has(l)) {
            leftOccuranceMap.set(l, leftOccuranceMap.get(l)+1);
        } else {
            leftOccuranceMap.set(l, 1);
        }

        if (rightOccuranceMap.has(r)) {
            rightOccuranceMap.set(r, rightOccuranceMap.get(r)+1);
        } else {
            rightOccuranceMap.set(r, 1);
        }

    }
 
    var similarityScore: number = 0;
    for (const l of left) {
        if (rightOccuranceMap.has(l))
            similarityScore += l * rightOccuranceMap.get(l);
    }    
   
    var sumOfDifferences: number = 0;
    while (left.length !== 0) {
        const leftPair = minAndIndex(left);
        const rightPair = minAndIndex(right);
    
        if (leftPair == null || rightPair == null)
            return null
        
        const [leftMin, leftIndex] = leftPair;
        const [rightMin, rightIndex] = rightPair;

        const diff =
            Math.max(leftMin, rightMin)
            - Math.min(leftMin, rightMin);
        sumOfDifferences += diff;

        // oh no, O(n)! Use a min-heap instead for O(log n)
        left.splice(leftIndex, 1);
        right.splice(rightIndex, 1);
    }

    console.log("(part1) sum of differences = " + sumOfDifferences);
    console.log("(part2) similarity score = " + similarityScore);
}

const fs = require('node:fs');

fs.readFile("./input.day1", "utf8", (err:Error, contents:string) => {
    if (err) throw err;
    bothParts(contents);
});

