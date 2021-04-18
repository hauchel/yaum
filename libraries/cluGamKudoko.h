// from https://attractivechaos.github.io/plb/kudoku.html"},
#define ANZGAM  108 // this
const static unsigned char PROGMEM games [ANZGAM][82] = {
  {"................................................................................."},
  // Hardest 20:
  {"..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9"},
  {".......12........3..23..4....18....5.6..7.8.......9.....85.....9...4.5..47...6..."},
  {".2..5.7..4..1....68....3...2....8..3.4..2.5.....6...1...2.9.....9......57.4...9.."},
  {"........3..1..56...9..4..7......9.5.7.......8.5.4.2....8..2..9...35..1..6........"},
  {"12.3....435....1....4........54..2..6...7.........8.9...31..5.......9.7.....6...8"},
  {"1.......2.9.4...5...6...7...5.9.3.......7.......85..4.7.....6...3...9.8...2.....1"},
  {".......39.....1..5..3.5.8....8.9...6.7...2...1..4.......9.8..5..2....6..4..7....."},
  {"12.3.....4.....3....3.5......42..5......8...9.6...5.7...15..2......9..6......7..8"},
  {"..3..6.8....1..2......7...4..9..8.6..3..4...1.7.2.....3....5.....5...6..98.....5."},
  {"1.......9..67...2..8....4......75.3...5..2....6.3......9....8..6...4...1..25...6."},
  {"..9...4...7.3...2.8...6...71..8....6....1..7.....56...3....5..1.4.....9...2...7.."},
  {"....9..5..1.....3...23..7....45...7.8.....2.......64...9..1.....8..6......54....7"},
  {"4...3.......6..8..........1....5..9..8....6...7.2........1.27..5.3....4.9........"},
  {"7.8...3.....2.1...5.........4.....263...8.......1...9..9.6....4....7.5..........."},
  {"3.7.4...........918........4.....7.....16.......25..........38..9....5...2.6....."},
  {"........8..3...4...9..2..6.....79.......612...6.5.2.7...8...5...1.....2.4.5.....3"},
  {".......1.4.........2...........5.4.7..8...3....1.9....3..4..2...5.1........8.6..."},
  {".......12....35......6...7.7.....3.....4..8..1...........12.....8.....4..5....6.."},
  {"1.......2.9.4...5...6...7...5.3.4.......6........58.4...2...6...3...9.8.7.......1"},
  {".....1.2.3...4.5.....6....7..2.....1.8..9..3.4.....8..5....2....9..3.4....67....."},
  // Topn87:
  {"4...3.......6..8..........1....5..9..8....6...7.2........1.27..5.3....4.9........"},
  {"7.8...3.....2.1...5.........4.....263...8.......1...9..9.6....4....7.5..........."},
  {"3.7.4...........918........4.....7.....16.......25..........38..9....5...2.6....."},
  {"7.8...3.....6.1...5.........4.....263...8.......1...9..9.2....4....7.5..........."},
  {"5..7..6....38...........2..62.4............917............35.8.4.....1......9...."},
  {"4..7..6....38...........2..62.5............917............43.8.5.....1......9...."},
  {".4..1.2.......9.7..1..........43.6..8......5....2.....7.5..8......6..3..9........"},
  {"7.5.....2...4.1...3.........1.6..4..2...5...........9....37.....8....6...9.....8."},
  {".8..1......5....3.......4.....6.5.7.89....2.....3.....2.....1.9..67........4....."},
  {"......41.9..3.....3...5.....48..7..........62.1.......6..2....5.7....8......9...."},
  {"7.5.....2...4.1...3.........1.6..4..2...5...........9....37.....9....8...8.....6."},
  {"8.9...3.....7.1...5.........7.....263...9.......1...4..6.2....4....8.5..........."},
  {"1...48....5....9....6...3.....57.2..8.3.........9............4167..........2....."},
  {"6.9.....8...7.1...4............6...4.2.....3..3....5...1.5...7.8...9..........2.."},
  {"8.5.....2...9.1...3.........6.7..4..2...5...........6....38.....1....9...4.....7."},
  {"......41.9..3.....3...2.....48..7..........52.1.......5..2....6.7....8......9...."},
  {"4.3.....2...6.1...8...........5..97.2...3.....1..........84.....9....6...7.....5."},
  {"..1.....7...89..........6..26..3.......5...749...........1.4.5.83.............2.."},
  {"3.7..4.2....1..8..9............3..9..5.8......4.6...........5.12...7..........6.."},
  {"......41.9..3.....3...5.....48..7..........52.1.......6..2....5.7....8......9...."},
  {"4.3.....2...6.1...8...........5..79.2...3.....1..........84.....9....6...7.....5."},
  {"....2..4..7...6....1.5.....2......8....3..7..4.9.........6..1.38...9..........5.."},
  {"7.8...3.....6.1...4.........6.....253...8.......1...9..9.5....2....7.4..........."},
  {"8.5.....2...9.1...3.........6.7..4..2...5...........6....38.....4....7...1.....9."},
  {"8.5.....2...9.1...3.........6.7..4..2...5...........6....38.....1....7...4.....9."},
  {"2...4.5...1.....3............6...8.2.7.3.9......1.....4...5.6.....7...9...8......"},
  {".......71.2.8........5.3...7.9.6.......2..8..1.........3...25..6...1..4.........."},
  {"7.4.....2...8.1...3.........5.6..1..2...4...........9....37.....8....6...9.....5."},
  {"....4...1.3.6.....8........1.9..5.........87....2......7....26.5...94.........3.."},
  {"8.5.....2...4.1...3.........6.7..4..2...5...........9....38.....1....7...9.....6."},
  {".1.62....5......43....9....7......8...5.....4...1..........36...9....2..8....7..."},
  {"7.4.....2...8.1...3.........5.6..1..2...4...........9....37.....9....5...8.....6."},
  {"...3.9.7.8..4.....1........2..5..6...3.....4.....1....5.....8......2.1.....7....9"},
  {"..36......4.....8.9.....7..86.4...........1.5.2.......5...17...1...9...........2."},
  {".......91.7..3....82..........1.5...3.....7.....9.......16...5...4.2....7.....8.."},
  {".8.....63....4.2............1.8.35..7.....9.....6.....2.9.7...........354........"},
  {"8.5.....2...9.1...3.........6.7..4..2...5...........6....38.....4....6...9.....7."},
  {".5.4.9......6....12.....3..7.3...2.....5...9.1.........68....4.....8........7...."},
  {"3..8.1....5....6.9......4..5..7...8..4..6...........2.2..3.........9.1....7......"},
  {".4.7...6...39............57.......3.2...8.....19...57.6...4.....5.1......2...6.84"},
  {"7.4.....2...8.1...3.........5.6..1..2...4...........5....37.....9....6...8.....9."},
  {"5..6.3....2....98.......1...1..9.......3....67.......4....8.25.4..7.............."},
  {"2.8.5.......7...4.3........5...2.9.......1......6......7.1.4.6.......3.2.1......."},
  {"...9.31..5.7....8.2.........4....6......5..2..1.......8...7.......6..4.....3....9"},
  {"......41.6..3.....3...2.....49..8..........52.1.......5..6....7.8....9......3...."},
  {"7.....48....6.1..........2....3..6.52...8..............53.....1.6.1.........4.7.."},
  {"5.8.....7...9.1...4............5...4.6.....3..9....6...2.3...1.7...8..........2.."},
  {"2...6...8.743.........2....62......1...4..5..8...........5..34......1..........7."},
  {"6.9.....8...3.1...4............6...4.2.....3..7....5...1.5...7.8...9..........2.."},
  {".6..5.4.3.2.1...........7..4.3...6..7..5........2.........8..5.6...4...........1."},
  {"5.7....3.....61...1.8......62..4.......7...8...........1....6.43..5...........2.."},
  {"4.3.....2...6.1...8...........5..97.2...3.....7..........84.....9....6...1.....5."},
  {"8.5.....2...4.1...3.........6.7..4..2...5...........6....38.....9....7...1.....9."},
  {".....1..8.9....3..2........5......84.7.63.......9.....1.4....5.....7.6.....2....."},
  {"......41.9..2.....3...5.....48..7..........62.1.......6..5....3.7....8......9...."},
  {".6..2...1...3...7..1.......3.49.....7.....2........5.8....586.........4.9........"},
  {"7.....4...2..7..8...3..8..9...5..3...6..2..9...1..7..6...3..9...3..4..6...9..1..5"},
  {"...9.31..6.7....8.2.........5....4......6..2..1.......8...7.......3..5.....4....9"},
  {"6..1...8..53.............4....8...6..9....7....24.........7.3.9....2.5..1........"},
  {"4.3.....2...7.1...9...........5..81.2...3.....8..........94.....7....6...6.....5."},
  {"4.3.....2...7.1...9...........5..18.2...3.....8..........94.....7....6...6.....5."},
  {"1..46...5.2....7......9.....3.7.8..........91...2........3..84.6........5........"},
  {"4.35...2.....61...7............895.....3..8..2...........4...7..9....6...1......."},
  {".6..2...1...3...7..1.......3.49.....7.....2........5.8....856.........4.9........"},
  {"3.7..4.2....1..5..9............3..9..5.8......4.6...........8.12...7..........6.."},
  {"4.1.6....3.....2........8..15.2.....6......1....9......2.7.8..........43.7......."},
  {"...8...3...5...7.....1.........5.9..18.......3..4.......7..2..6....7.5...4.....1."},
  {"7.....48....6.1..........2....3..6.52...8..............63.....1.5.1.........4.7.."},
  {"48.3............71.2.......7.5....6....2..8.............1.76...3.....4......5...."},
  {"4.3.....2...6.1...8...........5..79.2...3.....7..........84.....9....6...1.....5."},
  {".5..7..83..4....6.....5....83.6........9..1...........5.7...4.....3.2...1........"},
  {"....3..715..4.2............2..6..4...38.7..............7..8..1.6..5..2..........."},
  {".7.3...6.....8.5...1.......8.96..4.....1.2...5...........7...324...9............."},
  {"56..2......3...9...............7..561......2...84........3.84..71..........9....."},
  {".9.3...2.....7.5...1.......7.86..4.....9.2...5...........1...634...8............."},
  {"7.8.2...........913.........46..........3.7.....5......5.9.6......4...1.2.....8.."},
  {"7...3........5.6....4....9.2.....7.1...9.8......4.....53....2.....1...8..6......."},
  {"..3...67.5.....3...4.......6..3......8......4...7....12......5.....98.......41..."},
  {"4.35...2.....16...7............895.....3..8..2...........4...7..9....6...1......."},
  {".2.3...6.....7.5...1.......7.86..4.....9.2...5...........1...394...8............."},
  {"......41.9..3.....3...2.....48..7..........62.1.......5..2....6.7....8......9...."},
  {"6.....7.5.3.8................52.3.8.1.9.........4.....42...........9.1......7.6.."},
  {".5.1.8.7.4..3.....2.........1.7...8.9.....4............3.....1.....4.2......5.6.."},
  {"...6..9.23.87.....4............95..17......8...........2..6.5.....4...3..1......."},
  {"8.5.....2...4.1...3.........6.7..4..2...5...........6....38.....1....9...9.....7."},
  {"...6.37...51...........2.......1..546..7............8.14.58....3.....2..........."},
  {"..1.....8...9..2.......3.......15.4..6....7..3............4..8572.6.....9........"},
};
