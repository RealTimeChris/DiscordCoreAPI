# Json-Performance
Performance profiling of JSON libraries

Latest results (March 18, 2023):

Single Iteration Test Results:

| Library                                           | Test       | Serialize (MB/s) | Parse (MB/s) |
| ------------------------------------------------- | ---------- | ------------ | ----------- |
| [**glaze**](https://github.com/stephenberry/glaze) | **Mixed** | **133** | **25** |
| [**Jsonifier**](https://github.com/RealTimechris/Jsonifier) | **Mixed** | **409** | **48** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Mixed** | **N/A** | **21** |

Multi Iteration Test Results:

| Library                                           | Test       | Serialize (MB/s) | Parse (MB/s) |
| ------------------------------------------------- | ---------- | ------------ | ----------- |
| [**glaze**](https://github.com/stephenberry/glaze) | **Mixed** | **545** | **551** |
| [**glaze**](https://github.com/stephenberry/glaze) | **Double** | **568** | **1053** |
| [**glaze**](https://github.com/stephenberry/glaze) | **String** | **549** | **1392** |
| [**glaze**](https://github.com/stephenberry/glaze) | **Uint64** | **1538** | **1084** |
| [**glaze**](https://github.com/stephenberry/glaze) | **Int64** | **1637** | **1115** |
| [**Jsonifier**](https://github.com/RealTimechris/Jsonifier) | **Mixed** | **625** | **820** |
| [**Jsonifier**](https://github.com/RealTimechris/Jsonifier) | **Double** | **571** | **1102** |
| [**Jsonifier**](https://github.com/RealTimechris/Jsonifier) | **String** | **520** | **1568** |
| [**Jsonifier**](https://github.com/RealTimechris/Jsonifier) | **Uint64** | **1738** | **1101** |
| [**Jsonifier**](https://github.com/RealTimechris/Jsonifier) | **Int64** | **1602** | **1160** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Mixed** | **N/A** | **258** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Double** | **N/A** | **287** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **String** | **N/A** | **158** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Uint64** | **N/A** | **363** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Int64** | **N/A** | **348** |
>  1000 iterations on a 6 core (Intel i7 8700k)

Test object, used in the "Mixed" tests (minified for test):

```json
{
   "fixed_object": {
      "int_array": [0, 1, 2, 3, 4, 5, 6],
      "float_array": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6],
      "double_array": [3288398.238, 233e22, 289e-1, 0.928759872, 0.22222848, 0.1, 0.2, 0.3, 0.4,
                        3288398.238, 233e22, 289e-1, 0.928759872, 0.22222848, 0.1, 0.2, 0.3, 0.4,
                        3288398.238, 233e22, 289e-1, 0.928759872, 0.22222848, 0.1, 0.2, 0.3, 0.4,
                        3288398.238, 233e22, 289e-1, 0.928759872, 0.22222848, 0.1, 0.2, 0.3, 0.4]
   },
   "fixed_name_object": {
      "name0": "James",
      "name1": "Abraham",
      "name2": "Susan",
      "name3": "Frank",
      "name4": "Alicia"
   },
   "another_object": {
      "string": "here is some text",
      "another_string": "Hello World",
      "boolean": false,
      "nested_object": {
         "v3s": [[0.12345, 0.23456, 0.001345],
                  [0.3894675, 97.39827, 297.92387],
                  [18.18, 87.289, 2988.298],
                  [0.3894675, 97.39827, 297.92387],
                  [18.18, 87.289, 2988.298],
                  [0.3894675, 97.39827, 297.92387],
                  [18.18, 87.289, 2988.298]],
         "id": "298728949872"
      }
   },
   "string_array": ["Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger",
					"Cat", "Dog", "Elephant", "Tiger"],
   "string": "Hello world",
   "number": 3.14,
   "boolean": true,
   "another_bool": false
}
```

Test objects, used in the "Double, String, Int64, and Uint64" tests:

```cpp
template<typename ValueType> struct Test {
	std::vector<ValueType> a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z;

	Test() {
		auto fill = [](auto& v) {
			v.resize(1000);
			for (size_t x = 0; x < 1000; ++x) {
				v[x] = static_cast<ValueType>(1000000000000000000);
			}
		};

		fill(a);
		fill(b);
		fill(c);
		fill(d);
		fill(e);
		fill(f);
		fill(g);
		fill(h);
		fill(i);
		fill(j);
		fill(k);
		fill(l);
		fill(m);
		fill(n);
		fill(o);
		fill(p);
		fill(q);
		fill(r);
		fill(s);
		fill(t);
		fill(u);
		fill(v);
		fill(w);
		fill(x);
		fill(y);
		fill(z);
	}
};

template<> struct Test<std::string> {
	std::vector<std::string> a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z;

	Test() {
		auto fill = [](auto& v) {
			v.resize(1000);
			for (size_t x = 0; x < 1000; ++x) {
				v[x] = std::to_string(1000000000000000000);
			}
		};

		fill(a);
		fill(b);
		fill(c);
		fill(d);
		fill(e);
		fill(f);
		fill(g);
		fill(h);
		fill(i);
		fill(j);
		fill(k);
		fill(l);
		fill(m);
		fill(n);
		fill(o);
		fill(p);
		fill(q);
		fill(r);
		fill(s);
		fill(t);
		fill(u);
		fill(v);
		fill(w);
		fill(x);
		fill(y);
		fill(z);
	}
};
```

## ABC Test (Out of Sequence Performance)

In this test the JSON document has keys from "a" to "z", where each key refers to an array of values. The document orders the keys from "z" to "a", in reverse order to the Expected "a" to "z" layout.

This test demonstrates problems with `simdjson` and iterative parsers that cannot hash for memory locations. If keys are not in the Expected sequence performance seriously suffers, and the problem increases as the size of the document increases.

Hash based solutions avoid this problem and do not suffer performance loss as the JSON document grows in size.

| Library                                           | Test       | Serialize (MB/s) | Parse (MB/s) |
| ------------------------------------------------- | ---------- | ------------ | ----------- |
| [**glaze**](https://github.com/stephenberry/glaze) | **Double** | **575** | **941** |
| [**glaze**](https://github.com/stephenberry/glaze) | **String** | **539** | **1297** |
| [**glaze**](https://github.com/stephenberry/glaze) | **Uint64** | **1690** | **1117** |
| [**glaze**](https://github.com/stephenberry/glaze) | **Int64** | **1712** | **1111** |
| [**Jsonifier**](https://github.com/RealTimeChris/Jsonifier) | **Double** | **594** | **1084** |
| [**Jsonifier**](https://github.com/RealTimeChris/Jsonifier) | **String** | **525** | **1530** |
| [**Jsonifier**](https://github.com/RealTimeChris/Jsonifier) | **Uint64** | **1468** | **1181** |
| [**Jsonifier**](https://github.com/RealTimeChris/Jsonifier) | **Int64** | **1648** | **1185** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Double** | **N/A** | **130** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **String** | **N/A** | **99** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Uint64** | **N/A** | **141** |
| [**simdjson (on demand)**](https://github.com/simdjson/simdjson) | **Int64** | **N/A** | **147** |
>  1001 iterations on a 6 core (Intel i7 8700k)

# Registration Comparison

## Jsonifier

```c++
template<> struct Jsonifier::Core<fixed_object_t> {
	using ValueType = fixed_object_t;
	static constexpr auto value = object("int_array", &ValueType::int_array, "float_array", &ValueType::float_array, "double_array", &ValueType::double_array);
};

template<> struct Jsonifier::Core<fixed_name_object_t> {
	using ValueType = fixed_name_object_t;
	static constexpr auto value = object("name0", &ValueType::name0, "name1", &ValueType::name1, "name2", &ValueType::name2, "name3", &ValueType::name3, "name4", &ValueType::name4);
};

template<> struct Jsonifier::Core<nested_object_t> {
	using ValueType = nested_object_t;
	static constexpr auto value = object("v3s", &ValueType::v3s, "id", &ValueType::id);
};

template<> struct Jsonifier::Core<another_object_t> {
	using ValueType = another_object_t;
	static constexpr auto value =
		object("string", &ValueType::string, "another_string", &ValueType::another_string, "boolean", &ValueType::boolean, "nested_object", &ValueType::nested_object);
};

template<> struct Jsonifier::Core<obj_t> {
	using ValueType = obj_t;
	static constexpr auto value =
		object("fixed_object", &ValueType::fixed_object, "fixed_name_object", &ValueType::fixed_name_object, "another_object", &ValueType::another_object, "string_array",
			&ValueType::string_array, "string", &ValueType::string, "number", &ValueType::number, "boolean", &ValueType::boolean, "another_bool", &ValueType::another_bool);
};
```

## Glaze

```c++
template<> struct glz::meta<fixed_object_t> {
	using ValueType = fixed_object_t;
	static constexpr auto value = object("int_array", &ValueType::int_array, "float_array", &ValueType::float_array, "double_array", &ValueType::double_array);
};

template<> struct glz::meta<fixed_name_object_t> {
	using ValueType = fixed_name_object_t;
	static constexpr auto value = object("name0", &ValueType::name0, "name1", &ValueType::name1, "name2", &ValueType::name2, "name3", &ValueType::name3, "name4", &ValueType::name4);
};

template<> struct glz::meta<nested_object_t> {
	using ValueType = nested_object_t;
	static constexpr auto value = object("v3s", &ValueType::v3s, "id", &ValueType::id);
};

template<> struct glz::meta<another_object_t> {
	using ValueType = another_object_t;
	static constexpr auto value =
		object("string", &ValueType::string, "another_string", &ValueType::another_string, "boolean", &ValueType::boolean, "nested_object", &ValueType::nested_object);
};

template<> struct glz::meta<obj_t> {
	using ValueType = obj_t;
	static constexpr auto value =
		object("fixed_object", &ValueType::fixed_object, "fixed_name_object", &ValueType::fixed_name_object, "another_object", &ValueType::another_object, "string_array",
			&ValueType::string_array, "string", &ValueType::string, "number", &ValueType::number, "boolean", &ValueType::boolean, "another_bool", &ValueType::another_bool);
};
```

Or using macros:

```c++
GLZ_META(obj_t, fixed_object, fixed_name_object, another_object,
         string_array, string, number, boolean, another_bool);
```

## simdjson

```c++
simdjson::ondemand::parser parser{};
auto doc = parser.iterate(json);
ondemand::object fixed_object = doc["fixed_object"];

ondemand::array int_array = fixed_object["int_array"];
obj.fixed_object.int_array.clear();
for (int64_t x: int_array) {
	obj.fixed_object.int_array.emplace_back(x);
}

ondemand::array float_array = fixed_object["float_array"];
obj.fixed_object.float_array.clear();
// doesn't have a direct float conversion
for (double x: float_array) {
	obj.fixed_object.float_array.emplace_back(static_cast<float>(x));
}

ondemand::array double_array = fixed_object["double_array"];
obj.fixed_object.double_array.clear();
for (double x: double_array) {
	obj.fixed_object.double_array.emplace_back(x);
}

ondemand::object fixed_name_object = doc["fixed_name_object"];
obj.fixed_name_object.name0 = std::string_view(fixed_name_object["name0"]);
obj.fixed_name_object.name1 = std::string_view(fixed_name_object["name1"]);
obj.fixed_name_object.name2 = std::string_view(fixed_name_object["name2"]);
obj.fixed_name_object.name3 = std::string_view(fixed_name_object["name3"]);
obj.fixed_name_object.name4 = std::string_view(fixed_name_object["name4"]);

ondemand::object another_object = doc["another_object"];
obj.another_object.string = std::string_view(another_object["string"]);
obj.another_object.another_string = std::string_view(another_object["another_string"]);
obj.another_object.boolean = bool(another_object["boolean"]);

ondemand::object nested_object = another_object["nested_object"];
ondemand::array v3s = nested_object["v3s"];
obj.another_object.nested_object.v3s.clear();
for (ondemand::array v3: v3s) {
	size_t i = 0;
	auto& back = obj.another_object.nested_object.v3s.emplace_back();
	for (double x: v3) {
		back[i++] = x;
	}
}

obj.another_object.nested_object.id = std::string_view(nested_object["id"]);

ondemand::array string_array = doc["string_array"];
obj.string_array.resize(string_array.count_elements());
size_t index = 0;
for (std::string_view x: string_array) {
	obj.string_array[index++] = x;
}

obj.string = std::string_view(doc["string"]);
obj.number = double(doc["number"]);
obj.boolean = bool(doc["boolean"]);
obj.another_bool = bool(doc["another_bool"]);
```````
