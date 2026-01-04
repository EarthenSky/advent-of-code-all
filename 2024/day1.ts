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

function part1(contents: string): number | null {
    const pairs: [number, number][] =
        contents
        .split("\n")
        .map(str => str.split("   "))
        .map(str => [
            Number(str[0]),
            Number(str[1])
        ]);
  
    const leftIndexMap = new Map();
    const rightIndexMap = new Map();
    const left: number[] = [];
    const right: number[] = [];
    var i: number = 0;
    for (const [l, r] of pairs) {
        left.push(l);
        right.push(r);
        leftIndexMap.set(l, i);
        rightIndexMap.set(r, i);
        i += 1;
    }
    
    var sumOfDifferences: number = 0;
    while (left.length !== 0) {
        const leftPair = minAndIndex(left);
        const rightPair = minAndIndex(right);
    
        if (leftPair == null || rightPair == null)
            return null
        
        const [leftMin, leftIndex] = leftPair;
        const [rightMin, rightIndex] = rightPair;
        const [leftVal, rightVal] = [left[leftIndex], right[rightIndex]];

        console.log(leftMin);
        console.log(rightMin);

        if (leftMin != rightMin) {
            console.log("ERROR: expected both lists to have the same elements. Found differen min()");
            return null;
        }

        const diff =
            Math.max(leftIndexMap.get(leftVal), rightIndexMap.get(rightVal))
            - Math.min(leftIndexMap.get(leftVal), rightIndexMap.get(rightVal));
        console.log(diff);

        // oh no, O(n)! Use a min-heap instead for O(n log n)
        left.splice(leftIndex, 1);
        right.splice(rightIndex, 1);

        sumOfDifferences += diff;
    }

    return sumOfDifferences;
}

const totalDifferences = part1("9999   1234\n1234   1111\n1111   9999");
console.log(totalDifferences);
